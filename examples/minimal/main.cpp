//
// Created by Daniel Walz on 08.08.24.
//

#include <foleys_License.h>


constexpr auto localStorage = "~/Library/Application Support/Manufacturer/TestStandalone.lic";
constexpr auto hardwareUid  = "Hardware TODO";
constexpr auto os           = "OS TODO";
constexpr auto host         = "Host TODO";


int main (int, char**)
{

    foleys::License license;

    license.setupLicenseData (localStorage, hardwareUid, { { LicenseID::hardware, hardwareUid }, { LicenseID::os, os }, { LicenseID::host, host } });

    license.syncLicense();

    return 0;
}
