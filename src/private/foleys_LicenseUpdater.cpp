/********************************************************************************
Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************/


#include "foleys_LicenseUpdater.h"
#include "foleys_LicenseData.h"
#include "foleys_License.h"
#include "foleys_NetworkRequest.h"
#include "foleys_Crypto.h"
#include "foleys_Checks.h"

#include <nlohmann/json.hpp>
#include <ghc/filesystem.hpp>
#include <fstream>


namespace foleys
{

LicenseUpdater::LicenseUpdater()
{
    fetchIfNecessary();
}

void LicenseUpdater::setupLicenseData (const std::string& licenseFile, std::string_view hwUID, std::initializer_list<std::pair<std::string, std::string>> data)
{
    localStorage = licenseFile;
    hardwareUid  = hwUID;
    defaultData  = data;

    fetchIfNecessary();
}

void LicenseUpdater::fetchIfNecessary (int hours)
{
    if (hardwareUid.empty())
        return;

    auto contents = getContents();
    if (contents.empty())
    {
        fetchLicenseData();
        return;
    }

    auto json = nlohmann::json::parse (getContents(), nullptr, false);
    if (json.is_discarded())
    {
        fetchLicenseData();
        return;
    }

    auto timestamp = License::decodeDateTime (json[LicenseID::checked], "%Y-%m-%dT%H:%M:%S");
    auto seconds   = std::difftime (std::time (nullptr), timestamp);

    if (seconds > 3600 * hours)
        fetchLicenseData();
}


void LicenseUpdater::fetchLicenseData (std::string_view action, const std::vector<std::pair<std::string, std::string>>& data)
{
    if (hardwareUid.empty())
        return;

    nlohmann::json requestData;
    if (!action.empty())
        requestData[LicenseID::action] = action;

    requestData[LicenseID::product]  = LicenseData::productUid;
    requestData[LicenseID::hardware] = hardwareUid;

    for (const auto& pair: defaultData)
        requestData[pair.first] = pair.second;

    for (const auto& pair: data)
        requestData[pair.first] = pair.second;


    std::string url (LicenseData::authServerUrl);
#if FOLEYS_LICENSE_ENCRYPT_REQUEST
    auto payload = Crypto::encrypt (requestData.dump());
    url += "auth/" + LicenseData::productUid + "/";
#else
    auto payload = requestData.dump();
    url += "auth/";
#endif

    auto newRequest = std::make_unique<NetworkRequest> (url);

    newRequest->callback = [this] (int status, std::string_view downloaded)
    {
        if (status > 399)
        {
            lastError       = Licensing::Error::ServerNotAvailable;
            lastErrorString = "Server not reachable or timed out";
        }

        auto answer = Crypto::decrypt (downloaded);

        if (answer.empty())
        {
            lastError       = Licensing::Error::ServerAnswerInvalid;
            lastErrorString = "Server Error (could not decrypt)";
            sendUpdateSignal();
            return;
        }

        lastError       = Licensing::Error::NoError;
        lastErrorString = "";

        auto json = nlohmann::json::parse (answer, nullptr, false);
        if (json.is_discarded())
        {
            lastError       = Licensing::Error::ServerAnswerInvalid;
            lastErrorString = "Server Error (invalid json)";
            sendUpdateSignal();
            return;
        }

        if (Checks::getHardwareUID (json) != hardwareUid)
        {
            lastError       = Licensing::Error::ServerAnswerInvalid;
            lastErrorString = "License not applicable";
            sendUpdateSignal();
            return;
        }

        {
            const std::scoped_lock mutex (storageMutex);

            if (localStorage.empty())
            {
                lastError       = Licensing::Error::CouldNotSave;
                lastErrorString = "Could not open license file for writing";
                sendUpdateSignal();
                return;
            }

            ghc::filesystem::path filepath (localStorage);
            ghc::filesystem::create_directories (filepath.parent_path());
            std::ofstream output (localStorage);
            if (output.is_open())
            {
                output << downloaded;
            }
            else
            {
                lastError       = Licensing::Error::CouldNotSave;
                lastErrorString = "Could not open license file for writing";
            }
        }

        if (json.contains (LicenseID::error))
        {
            lastError       = Licensing::Error::ServerError;
            lastErrorString = json[LicenseID::error];
        }

        sendUpdateSignal();
    };

    newRequest->fetch (payload);
    request = std::move (newRequest);
}

std::vector<Licensing::Activation> LicenseUpdater::getActivations()
{
    auto json = nlohmann::json::parse (getContents(), nullptr, false);
    if (json.is_discarded())
        return {};

    if (json.contains (LicenseID::activations))
    {
        std::vector<Licensing::Activation> acts;
        for (const auto& a: json[LicenseID::activations])
            acts.push_back ({ a[LicenseID::id], a[LicenseID::computer], a[LicenseID::user] });

        return acts;
    }

    return {};
}

std::string LicenseUpdater::getContents()
{
    if (localStorage.empty())
        return {};

    const std::scoped_lock mutex (storageMutex);

    std::ifstream input (localStorage);
    std::string   cipher (std::istreambuf_iterator<char> { input }, {});

    if (cipher.empty())
        return {};

    return Crypto::decrypt (cipher);
}

void LicenseUpdater::sendUpdateSignal()
{
    observerList.call ([] (auto& l) { l.licenseUpdated(); });
}

void LicenseUpdater::addObserver (Observer* observer)
{
    observerList.addObserver (observer);
}

void LicenseUpdater::removeObserver (Observer* observer)
{
    observerList.removeObserver (observer);
}

}  // namespace foleys
