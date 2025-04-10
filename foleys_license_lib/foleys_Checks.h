//
// Created by Daniel Walz on 04.10.24.
//

#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_CHECKS_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_CHECKS_H


#include "foleys_license_common/foleys_LicenseData.h"

#include <string>
#include <nlohmann/json.hpp>

namespace foleys::Checks
{

static std::string getHardwareUID (const nlohmann::json& json)
{
    if (json.contains (LicenseID::hardware))
        return json[LicenseID::hardware];

    return {};
}

}

#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_CHECKS_H
