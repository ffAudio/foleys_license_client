/********************************************************************************
Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************/

#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_JUCEUTILITIES_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_JUCEUTILITIES_H

#include "private/foleys_LicenseData.h"

#include <juce_core/juce_core.h>
#include <filesystem>

namespace foleys
{

static inline std::string createLicensePath (const char* manufacturer, const char* productName)
{
    auto appFolder = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory);
#if JUCE_MAC
    appFolder = appFolder.getChildFile ("Application Support");
#endif
    return appFolder.getChildFile (manufacturer).getChildFile (productName).withFileExtension (".lic").getFullPathName().toRawUTF8();
}

}  // namespace foleys

#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_JUCEUTILITIES_H
