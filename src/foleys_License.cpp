//
// Created by Daniel Walz on 07.08.24.
//

#include "foleys_License.h"

#include "private/foleys_LicenseData.h"

#include <private/choc_Base64.h>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <sodium.h>

#include <iostream>

namespace foleys
{

Licensing::Licensing() = default;

void Licensing::reload()
{
    loadLicenseBlob();

    if (lastCheckExpired())
        syncLicense();
}

void Licensing::syncLicense()
{
    fetchLicenseData();
}

bool Licensing::checkHardwareUid() const
{
    return licenseHardware == hardwareUid;
}

bool Licensing::activated() const
{
    return activatedFlag.load();
}

bool Licensing::expired() const
{
    if (expiryDate)
        return *expiryDate < std::time (nullptr);

    return false;
}

bool Licensing::isAllowed() const
{
    return checkHardwareUid() && ((activated() && !expired()) || isDemo());
}

std::string Licensing::getLastError() const
{
    return lastErrorString;
}

std::optional<std::time_t> Licensing::expires() const
{
    return expiryDate;
}

std::string Licensing::getLicenseeEmail() const
{
    std::scoped_lock guard (processingLock);
    return email;
}

void Licensing::login (const std::string& login_email)
{
    fetchLicenseData ("login", { { "login_email", login_email } });
}

void Licensing::activate (std::initializer_list<std::pair<std::string, std::string>> data)
{
    fetchLicenseData ("activate", data);
}

bool Licensing::canDemo() const
{
    return demoAvailable;
}

bool Licensing::isDemo() const
{
    return !demoAvailable && demoDays > 0;
}

int Licensing::demoDaysLeft() const
{
    return demoDays;
}

void Licensing::startDemo()
{
    fetchLicenseData ("demo");
}

void Licensing::setLocalStorage (const std::filesystem::path& path)
{
    localStorage = path;
    if (std::filesystem::exists (localStorage))
        loadLicenseBlob();
}

void Licensing::setHardwareUid (std::string_view uid)
{
    hardwareUid = uid;
}

// ================================================================================

void Licensing::fetchLicenseData (std::string_view action, std::initializer_list<std::pair<std::string, std::string>> data)
{
    nlohmann::json request;
    request["product"]  = LicenseData::productUid;
    request["hardware"] = hardwareUid;
    if (!action.empty())
        request["action"] = action;

    for (const auto& pair: data)
        request[pair.first] = pair.second;

    cpr::Response r = cpr::Post (cpr::Url (LicenseData::authServerUrl) + "auth/", cpr::Body (request.dump()));

    if (processData (r.text))
    {
        std::filesystem::create_directories (localStorage.parent_path());
        std::ofstream output (localStorage);
        if (output.is_open())
        {
            output << r.text;
        }
        else
        {
            lastError = Error::CouldNotSave;
            lastErrorString = "Could not open license file for writing";
        }

        observerList.call ([] (auto& l) { l.licenseFetched(); });
    }
}

bool Licensing::processData (std::string_view data)
{
    std::scoped_lock guard (processingLock);

    const auto convert_time = [] (const std::string& timeString, const char* formatString)
    {
        std::tm            tm {};
        std::istringstream ss (timeString);
        ss >> std::get_time (&tm, formatString);
        return std::mktime (&tm);
    };

    std::vector<unsigned char> binary;
    if (!choc::base64::decodeToContainer (binary, data))
    {
        lastError       = Error::ServerAnswerInvalid;
        lastErrorString = "Got invalid license data (base64 decoding failed)";
        return false;
    }

    std::vector<unsigned char> message (binary.size());
    if (crypto_box_open_easy (message.data(), binary.data() + crypto_box_noncebytes(), binary.size() - crypto_box_noncebytes(), binary.data(),
                              LicenseData::publicKey, LicenseData::privateKey)
        != 0)
    {
        lastError       = Error::ServerAnswerInvalid;
        lastErrorString = "Got invalid license data (decryption failed)";
        return false;
    }

    auto response = nlohmann::json::parse (message, nullptr, false);
    if (response.is_discarded())
    {
        lastError       = Error::ServerAnswerInvalid;
        lastErrorString = "Got invalid license data (bad json)";
        return false;
    }

    checked       = convert_time (response["checked"], "%Y-%m-%dT%H:%M:%S");
    activatedFlag = response["activated"];
    email         = response.contains ("licensee_email") ? response["licensee_email"] : "";

    licenseHardware = response["hardware"];

    if (!checkHardwareUid())
    {
        activatedFlag   = false;
        lastError       = Error::HardwareMismatch;
        lastErrorString = "Hardware mismatch";
        return false;
    }

    if (response.contains ("license_expires"))
        expiryDate = convert_time (response["license_expires"], "%Y-%m-%d");
    else
        expiryDate = std::nullopt;

    if (response.contains ("demo_available"))
    {
        demoAvailable = response["demo_available"];
        demoDays      = response["demo_days"];
        if (response.contains ("demo_ends"))
        {
            auto ends          = convert_time (response["demo_ends"], "%Y-%m-%d");
            auto localDemoDays = static_cast<int> (1 + difftime (ends, std::time (nullptr)) / (24 * 3600));
            demoDays           = std::min (demoDays.load(), localDemoDays);
        }
    }
    else
    {
        demoAvailable = false;
        demoDays      = 0;
    }

    if (response.contains ("error"))
    {
        lastError       = Error::ServerAnswerInvalid;
        lastErrorString = response["error"];
    }
    else
    {
        lastError = Error::NoError;
        lastErrorString.clear();
    }

    return true;
}

void Licensing::loadLicenseBlob()
{
    std::ifstream input (localStorage);
    std::string   text (std::istreambuf_iterator<char> { input }, {});

    if (text.empty())
    {
        lastError       = Error::CouldNotRead;
        lastErrorString = "No local license file";
        fetchLicenseData();
    }
    else if (!processData (text))
        fetchLicenseData();
    else
        observerList.call ([] (auto& l) { l.licenseLoaded(); });
}

bool Licensing::lastCheckExpired() const
{
    auto seconds = std::difftime (std::time (nullptr), checked);
    return seconds > 3600 * 24;
}

void Licensing::addObserver (Observer* observer)
{
    observerList.addObserver (observer);
}

void Licensing::removeObserver (Observer* observer)
{
    observerList.removeObserver (observer);
}


}  // namespace foleys
