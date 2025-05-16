/********************************************************************************
Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************/


#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEDATA_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEDATA_H

namespace LicenseID
{
static constexpr const char* action          = "action";
static constexpr const char* checked         = "checked";
static constexpr const char* error           = "error";
static constexpr const char* product         = "product";
static constexpr const char* hardware        = "hardware";
static constexpr const char* version         = "version";
static constexpr const char* email           = "email";
static constexpr const char* serial          = "serial";
static constexpr const char* host            = "host";
static constexpr const char* id              = "id";
static constexpr const char* os              = "os";
static constexpr const char* computer        = "computer";
static constexpr const char* user            = "user";
static constexpr const char* activated       = "activated";
static constexpr const char* activations     = "activations";
static constexpr const char* demo            = "demo";
static constexpr const char* login           = "sendLoginEmail";
static constexpr const char* activate        = "activate";
static constexpr const char* deactivate      = "deactivate";
static constexpr const char* license_expires = "license_expires";
static constexpr const char* licensee_email  = "licensee_email";
static constexpr const char* demo_available  = "demo_available";
static constexpr const char* demo_days       = "demo_days";
static constexpr const char* demo_ends       = "demo_ends";
}  // namespace LicenseID


namespace LicenseData
{
extern const char*          productName;
extern const char*          copyright;
extern const char*          version;
extern const char*          authServerUrl;  //< license check backend
extern const char*          buyUrl;
extern const char*          manualUrl;
extern const char*          productUid;
extern const unsigned char* publicKey;   //< server's public
extern const unsigned char* privateKey;  //< client's private
}  // namespace LicenseData


#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEDATA_H
