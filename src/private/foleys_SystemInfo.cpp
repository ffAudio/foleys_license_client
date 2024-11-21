/********************************************************************************
    Copyright 2024 - Daniel Walz
    Foleys Finest Audio UG (haftungsbeschraenkt)
    Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

    This code is provided under the ISC license

    This code is provided as is. The authors disclaim all warranties.
    For details refer to the LICENSE.md

*******************************************************************************/


#include "foleys_SystemInfo.h"

#include <platform_folders.h>
#include <ghc/filesystem.hpp>

namespace foleys
{

std::string SystemInfo::getAppData()
{
    return sago::getDataHome();
}

std::string SystemInfo::createLicensePath (const char* manufacturer, const char* productName)
{
    ghc::filesystem::path appFolder (getAppData());
    std::string           filename (productName);
    filename += ".lic";
    return (appFolder / manufacturer / filename).string();
}

}  // namespace foleys
