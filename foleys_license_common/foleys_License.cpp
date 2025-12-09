/********************************************************************************
Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************/

#include "foleys_License.h"

#if FOLEYS_LICENSING_HAS_JUCE
    #include "../foleys_license_juce/Backend/foleys_LicensePimpl.cpp"
#else
    #include "../foleys_license_lib/foleys_LicensePimpl.cpp"
#endif


namespace foleys
{

License::License()
{
    pimpl = std::make_unique<Pimpl> (*this);
}

License::~License() = default;

void License::setupLicenseData (const FF_PATH& licenseFile, std::string_view hwUID, std::initializer_list<std::pair<std::string, FF_STRING>> data)
{
    pimpl->setupLicenseData (licenseFile, hwUID, data);
}

void License::syncLicense()
{
    pimpl->fetchLicenseData();
}

bool License::isActivated() const
{
    return pimpl->activatedFlag;
}

bool License::isExpired() const
{
    return pimpl->isExpired();
}

bool License::isAllowed() const
{
    return (pimpl->activatedFlag && !pimpl->isExpired()) || (pimpl->demoDays >= 0 && !pimpl->demoAvailable);
}

LicenseDefines::Error License::getLastError() const
{
    return pimpl->getLastError();
}

std::string License::getLastErrorString() const
{
    return pimpl->getLastErrorString();
}

std::optional<std::time_t> License::expires() const
{
    return pimpl->expiryDate;
}

std::optional<std::time_t> License::lastChecked() const
{
    return pimpl->checked;
}

bool License::lastActionWasActivate() const
{
    return pimpl->lastActionWasActivate;
}


void License::licenseChanged()
{
    if (onLicenseReceived)
        onLicenseReceived();
}

std::string License::getLicenseeEmail() const
{
    return pimpl->getLicenseeEmail();
}

void License::sendLoginEmail (const std::string& email)
{
    pimpl->fetchLicenseData (LicenseID::login, { { "login_email", email } });
}

void License::activate (const std::vector<std::pair<std::string, FF_STRING>>& data)
{
    pimpl->fetchLicenseData (LicenseID::activate, data);
}

void License::deactivate (size_t otherID, const std::vector<std::pair<std::string, FF_STRING>>& data)
{
    auto additionalData = data;
    additionalData.emplace_back (LicenseID::deactivate, std::to_string (otherID));
    pimpl->fetchLicenseData (LicenseID::activate, additionalData);
}

std::vector<Activation> License::getActivations() const
{
    return pimpl->getActivations();
}

bool License::canDemo() const
{
    return pimpl->demoAvailable;
}

bool License::isDemo() const
{
    return !pimpl->activatedFlag && pimpl->demoDays > 0 && !pimpl->demoAvailable;
}

int License::demoDaysLeft() const
{
    return pimpl->demoDays;
}

void License::startDemo()
{
    pimpl->fetchLicenseData (LicenseID::demo);
}

std::optional<std::time_t> License::getDemoEndDate() const
{
    return pimpl->demoEndDate;
}

bool License::shouldShowPopup()
{
    return pimpl->shouldShowPopup();
}

void License::setPopupWasShown (bool wasShown)
{
    pimpl->setPopupWasShown (wasShown);
}

std::string License::getOfflineRequest() const
{
    return pimpl->getOfflineRequest();
}

bool License::setOfflineLicenseData (std::string_view content)
{
    return pimpl->setOfflineLicenseData (content);
}

std::string License::getRawLicenseData() const
{
    return pimpl->getRawLicenseData();
}


// ================================================================================


}  // namespace foleys
