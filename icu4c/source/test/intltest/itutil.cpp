/*
*****************************************************************************************
*                                                                                       *
* COPYRIGHT:                                                                            *
*   (C) Copyright Taligent, Inc.,  1997                                                 *
*   (C) Copyright International Business Machines Corporation,  1997-1998               *
*   Licensed Material - Program-Property of IBM - All Rights Reserved.                  *
*   US Government Users Restricted Rights - Use, duplication, or disclosure             *
*   restricted by GSA ADP Schedule Contract with IBM Corp.                              *
*                                                                                       *
*****************************************************************************************
*/

/**
 * IntlTestUtilities is the medium level test class for everything in the directory "utility".
 */

#include "utypes.h"
#include "itutil.h"
#include "loctest.h"
#include "citrtest.h"
#include "ustrtest.h"
#include "ucdtest.h"
#include "restest.h"
#include "tsmutex.h"
#include "tsmthred.h"
#include "tsputil.h"
//#include "custrtest.h"
//#include "ccitrtst.h"
//#include "cloctest.h"
//#include "ctres.h"
//#include "ctucd.h"

void IntlTestUtilities::runIndexedTest( int32_t index, bool_t exec, char* &name, char* par )
{
    if (exec) logln("TestSuite Utilities: ");
    switch (index) {
        case 0:
            name = "LocaleTest"; 
            if (exec) {
                logln("LocaleTest---"); logln("");
                LocaleTest test;
                callTest( test, par );
            }
            break;

        case 1:
            name = "CharIterTest"; 
            if (exec) {
                logln("CharIterTest---"); logln("");
                CharIterTest test;
                callTest( test, par );
            }
            break;

        case 2:
            name = "UnicodeStringTest"; 
            if (exec) {
                logln("UnicodeStringTest---"); logln("");
                UnicodeStringTest test;
                callTest( test, par );
            }
            break;

        case 3:
            name = "UnicodeTest"; 
            if (exec) {
                logln("UnicodeTest---"); logln("");
                UnicodeTest test;
                callTest( test, par );
            }
            break;

        case 4:
            name = "ResourceBundleTest"; 
            if (exec) {
                logln("ResourceBundleTest---"); logln("");
                ResourceBundleTest test;
                callTest( test, par );
            }
            break;

        case 5:
            name = "MutexTest"; 
            if (exec) {
                logln("MutexTest---"); logln("");
                MutexTest test;
                callTest( test, par );
            }
            break;

        case 6:
            name = "MultithreadTest"; 
            if (exec) {
                logln("MultithreadTest---"); logln("");
                MultithreadTest test;
                callTest( test, par );
            }
            break;

        case 7:
            name = "PUtilTest"; 
            if (exec) {
                logln("PUtilTest---"); logln("");
                PUtilTest test;
                callTest( test, par );
            }
            break;
        /*
        case 8:
            name = "LocaleTest"; 
            if (exec) {
                logln("LocaleTest---"); logln("");
                CLocaleTest test;
                callTest( test, par );
            }
            break;


        case 9:
            name = "UnicodeStringCAPI";
            if (exec) {
                logln("UnicodeString C Round Trip test---"); logln("");
                CUnicodeStringTest test;
                callTest(test, par);
            }
            break;

        case 10:
            name = "CharacterIteratorCAPI";
            if (exec) {
                logln("CharacterIterator C Round Trip test---"); logln("");
                CCharIterTest test;
                callTest(test, par);
            }
            break;


        case 11:
            name = "UnicodeCAPI";
            if (exec) {
                logln("Unicode C-API test---"); logln();
                TestCwrapperUnicode test;
                callTest(test, par);
            }
            break;

        case 12:
            name = "ResourceBundleCAPI";
            if (exec) {
                logln("ResourceBundle C-API test---"); logln();
                TestCwrapperResourceBundle test;
                callTest(test, par);
            }
            break;
        */
        default: name = ""; break; //needed to end loop
    }
}

