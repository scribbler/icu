// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
package org.unicode.icu.tool.cldrtoicu.mapper;

import static org.unicode.cldr.api.AttributeKey.keyOf;
import static org.unicode.cldr.api.CldrData.PathOrder.DTD;
import static org.unicode.cldr.api.CldrDataSupplier.CldrResolution.UNRESOLVED;

import java.util.Optional;
import java.util.concurrent.atomic.AtomicBoolean;

import org.unicode.cldr.api.AttributeKey;
import org.unicode.cldr.api.CldrData;
import org.unicode.cldr.api.CldrData.PrefixVisitor;
import org.unicode.cldr.api.CldrDataSupplier;
import org.unicode.cldr.api.CldrDataType;
import org.unicode.cldr.api.CldrPath;
import org.unicode.icu.tool.cldrtoicu.IcuData;
import org.unicode.icu.tool.cldrtoicu.PathMatcher;
import org.unicode.icu.tool.cldrtoicu.RbPath;

import com.google.common.annotations.VisibleForTesting;
import com.google.common.escape.UnicodeEscaper;

/**
 * A mapper to collect plural data from {@link CldrDataType#LDML LDML} data via the paths:
 * <pre>{@code
 *   //ldml/rbnf/rulesetGrouping[@type=*]/ruleset[@type=*]
 * }</pre>
 */
// TODO: This class can almost certainly be written using RegexTransformer and a small config.
public final class RbnfMapper {
    private static final PathMatcher RULE_SET =
        PathMatcher.of("ldml/rbnf/rulesetGrouping[@type=*]/ruleset[@type=*]");
    private static final AttributeKey GROUPING_TYPE = keyOf("rulesetGrouping", "type");
    private static final AttributeKey RULESET_TYPE = keyOf("ruleset", "type");

    private static final PathMatcher RBNF_RULE = PathMatcher.of("rbnfrule");
    private static final AttributeKey RBNF_VALUE = keyOf("rbnfrule", "value");
    private static final AttributeKey RBNF_RADIX = keyOf("rbnfrule", "radix");
    private static final AttributeKey RULESET_ACCESS = keyOf("ruleset", "access");

    // This is the ICU path prefix, below which everything generated by this visitor will go.
    private static final RbPath RB_ROOT = RbPath.of("RBNFRules");

    /**
     * Processes data from the given supplier to generate RBNF data for a set of locale IDs.
     *
     * @param localeId the locale ID to generate data for.
     * @param src the CLDR data supplier to process.
     * @param icuSpecialData additional ICU data (in the "icu:" namespace)
     * @return IcuData containing RBNF data for the given locale ID.
     */
    public static IcuData process(
        String localeId, CldrDataSupplier src, Optional<CldrData> icuSpecialData) {

        return process(localeId, src.getDataForLocale(localeId, UNRESOLVED), icuSpecialData);
    }

    @VisibleForTesting // It's easier to supply a fake data instance than a fake supplier.
    static IcuData process(String localeId, CldrData cldrData, Optional<CldrData> icuSpecialData) {
        // Using DTD order is essential here because the RBNF paths contain ordered elements,
        // so we must ensure that they appear in sorted order (otherwise we'd have to do more
        // work at this end to re-sort the results).
        RulesetVisitor visitor = new RulesetVisitor(localeId);
        icuSpecialData.ifPresent(s -> s.accept(DTD, visitor));
        cldrData.accept(DTD, visitor);
        return visitor.icuData;
    }

    static final class RulesetVisitor implements PrefixVisitor {

        private final IcuData icuData;

        private RulesetVisitor(String localeId) {
            this.icuData = new IcuData(localeId, true);
        }

        @Override public void visitPrefixStart(CldrPath prefix, Context context) {
            if (RULE_SET.matchesPrefixOf(prefix)) {
                RbPath rbPath = RB_ROOT.extendBy(GROUPING_TYPE.valueFrom(prefix));
                String rulesetType = RULESET_TYPE.valueFrom(prefix);
                boolean isStrict = !"lenient-parse".equals(rulesetType);

                // This is rather hacky because the access attribute lives on the parent path
                // element, but we cannot use it until we visit the child values (because it's a
                // value attribute and will not be in the prefix path. So we need to add the header
                // only once, just before we start adding the values relating to the child
                // elements, so we need a flag.
                //
                // This cannot be a boolean field since it must be "effectively final".
                AtomicBoolean hasHeader = new AtomicBoolean(false);
                context.install(
                    value -> {
                        if (RBNF_RULE.matchesSuffixOf(value.getPath())) {
                            if (!hasHeader.get()) {
                                boolean isPrivate =
                                    RULESET_ACCESS.valueFrom(value, "public").equals("private");
                                icuData.add(rbPath, (isPrivate ? "%%" : "%") + rulesetType + ":");
                                hasHeader.set(true);
                            }
                            String rulePrefix = "";
                            if (isStrict) {
                                String basePrefix = RBNF_VALUE.valueFrom(value);
                                rulePrefix = RBNF_RADIX.optionalValueFrom(value)
                                    .map(r -> basePrefix + "/" + r)
                                    .orElse(basePrefix);
                                rulePrefix += ": ";
                            }
                            icuData.add(
                                rbPath,
                                rulePrefix + ESCAPE_RBNF_DATA.escape(value.getValue()));
                        }
                    });
            }
        }

        /*
         * Convert characters outside the range U+0020 to U+007F to Unicode escapes, and convert
         * backslash to a double backslash. This class is super slow for non-ASCII escaping due to
         * using "String.format()", however there's < 100 values that need any escaping, so it's
         * fine.
         */
        private static final UnicodeEscaper ESCAPE_RBNF_DATA = new UnicodeEscaper() {
            private final char[] DOUBLE_BACKSLASH = "\\\\".toCharArray();
            private final char[] LEFT_ANGLE = "<".toCharArray();
            private final char[] RIGHT_ANGLE = ">".toCharArray();

            @Override
            protected char[] escape(int cp) {
                // Returning null means "do not escape".
                switch (cp) {
                case '\\':
                    return DOUBLE_BACKSLASH;
                case '←':
                    return LEFT_ANGLE;
                case '→':
                    return RIGHT_ANGLE;
                default:
                    if (0x0020 <= cp && cp <= 0x007F) {
                        return null;
                    } else if (cp <= 0xFFFF) {
                        return String.format("\\u%04X", cp).toCharArray();
                    }
                    return String.format("\\U%08X", cp).toCharArray();
                }
            }
        };
    }
}
