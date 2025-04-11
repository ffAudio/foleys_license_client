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

void License::setupLicenseData (const std::string& licenseFile, std::string_view hwUID, std::initializer_list<std::pair<std::string, std::string>> data)
{
    pimpl->setupLicenseData (licenseFile, hwUID, data);
}

void License::syncLicense()
{
    pimpl->fetchLicenseData();
}

bool License::isActivated() const
{
    return activatedFlag.load();
}

bool License::isExpired() const
{
    return pimpl->isExpired();
}

bool License::isAllowed() const
{
    return allowedFlag;
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

void License::licenseChanged() const
{
    if (onLicenseReceived)
        onLicenseReceived();
}

std::string License::getLicenseeEmail() const
{
    return pimpl->getLicenseeEmail();
}

void License::login (const std::string& login_email)
{
    pimpl->fetchLicenseData (LicenseID::login, { { "login_email", login_email } });
}

void License::activate (const std::vector<std::pair<std::string, std::string>>& data)
{
    pimpl->fetchLicenseData (LicenseID::activate, data);
}

void License::deactivate (const std::vector<std::pair<std::string, std::string>>& data)
{
    pimpl->fetchLicenseData (LicenseID::deactivate, data);
}

std::vector<Activation> License::getActivations()
{
    return pimpl->getActivations();
}

bool License::canDemo() const
{
    return demoAvailable;
}

bool License::isDemo() const
{
    return pimpl->isDemo();
}

int License::demoDaysLeft() const
{
    return pimpl->demoDays;
}

void License::startDemo()
{
    pimpl->fetchLicenseData (LicenseID::demo);
}

bool License::shouldShowPopup()
{
    return pimpl->shouldShowPopup();
}

void License::setPopupWasShown (bool wasShown)
{
    pimpl->setPopupWasShown (wasShown);
}

// ================================================================================


}  // namespace foleys
