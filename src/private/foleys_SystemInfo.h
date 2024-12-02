/********************************************************************************
    Copyright 2024 - Daniel Walz
    Foleys Finest Audio UG (haftungsbeschraenkt)
    Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

    This code is provided under the ISC license

    This code is provided as is. The authors disclaim all warranties.
    For details refer to the LICENSE.md

*******************************************************************************/


#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_SYSTEMINFO_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_SYSTEMINFO_H


#include <string>

namespace foleys
{

struct SystemInfo
{

    /**
     * @return the application settings folder e.g. Application Support
     */
    static std::string getAppData();

    /**
     * @return the application settings folder e.g. Application Support, on windows the local
     */
    static std::string getLocalAppData();

    /**
     *
     * @param manufacturer the manufacturer name as folder name under APP_DATA
     * @param productName the product name as license filename
     * @param suffix the suffix for the license filename including the dot
     * @return a path name for the license file
     */
    static std::string createLicensePath (const char* manufacturer, const char* productName, const char* suffix = ".lic");
};

}  // namespace foleys


#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_SYSTEMINFO_H
