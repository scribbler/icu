/*
 * @(#)AnchorTables.cpp	1.6 00/03/15
 *
 * (C) Copyright IBM Corp. 1998, 1999, 2000 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEFontInstance.h"
#include "DeviceTables.h"
#include "AnchorTables.h"
#include "LESwaps.h"

void AnchorTable::getAnchor(LEGlyphID glyphID, LEFontInstance *fontInstance,
                            LEPoint &anchor)
{
    switch(SWAPW(anchorFormat)) {
    case 1:
    {
        Format1AnchorTable *f1 = (Format1AnchorTable *) this;

        f1->getAnchor(fontInstance, anchor);
        break;
    }

    case 2:
    {
        Format2AnchorTable *f2 = (Format2AnchorTable *) this;

        f2->getAnchor(glyphID, fontInstance, anchor);
        break;
    }

    case 3:
    {
        Format3AnchorTable *f3 = (Format3AnchorTable *) this;

        f3->getAnchor(fontInstance, anchor);
        break;
    }

    default:
        break;
    }
}

void Format1AnchorTable::getAnchor(LEFontInstance *fontInstance, LEPoint &anchor)
{
    le_int16 x = SWAPW(xCoordinate);
    le_int16 y = SWAPW(yCoordinate);
    LEPoint pixels;

    fontInstance->transformFunits(x, y, pixels);

    fontInstance->pixelsToUnits(pixels, anchor);
}

void Format2AnchorTable::getAnchor(LEGlyphID glyphID, LEFontInstance *fontInstance, LEPoint &anchor)
{
    LEPoint point;

    if (! fontInstance->getGlyphPoint(glyphID, SWAPW(anchorPoint), point)) {
        le_int16 x = SWAPW(xCoordinate);
        le_int16 y = SWAPW(yCoordinate);

        fontInstance->transformFunits(x, y, point);
    }


    fontInstance->pixelsToUnits(point, anchor);
}

void Format3AnchorTable::getAnchor(LEFontInstance *fontInstance, LEPoint &anchor)
{
    le_int16 x = SWAPW(xCoordinate);
    le_int16 y = SWAPW(yCoordinate);
    LEPoint pixels;
    Offset dtxOffset = SWAPW(xDeviceTableOffset);
    Offset dtyOffset = SWAPW(yDeviceTableOffset);

    fontInstance->transformFunits(x, y, pixels);

    if (dtxOffset != 0) {
        DeviceTable *dtx = (DeviceTable *) ((char *) this + dtxOffset);
        le_int16 adjx = dtx->getAdjustment((le_int16) fontInstance->getXPixelsPerEm());

        pixels.fX += adjx;
    }

    if (dtyOffset != 0) {
        DeviceTable *dty = (DeviceTable *) ((char *) this + dtyOffset);
        le_int16 adjy = dty->getAdjustment((le_int16) fontInstance->getYPixelsPerEm());

        pixels.fY += adjy;
    }

    fontInstance->pixelsToUnits(pixels, anchor);
}



