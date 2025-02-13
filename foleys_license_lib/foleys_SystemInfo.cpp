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

std::string SystemInfo::getLocalAppData()
{
    return sago::getStateDir();
}

std::string SystemInfo::createLicensePath (const char* manufacturer, const char* productName, const char* suffix)
{
    ghc::filesystem::path appFolder (getLocalAppData());
    std::string           filename (productName);
    filename += suffix;
    return (appFolder / manufacturer / filename).string();
}

}  // namespace foleys
