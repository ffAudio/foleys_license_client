//
// Created by Daniel Walz on 19.02.25.
//

#include "foleys_LicenseUpdaterJuce.h"
#include "foleys_license_common/foleys_LicenseDefines.h"
#include "foleys_license_common/foleys_LicenseData.h"
#include "foleys_license_common/foleys_Crypto.h"
#include "foleys_license_common/foleys_LicenseHelpers.h"

namespace foleys
{

LicenseUpdaterJuce::LicenseUpdaterJuce()
{
    // if this is called immediately, setupLicenseData had no chance
    // plus it will delay when scanning
    juce::WeakReference<LicenseUpdaterJuce> checker (this);
    juce::Timer::callAfterDelay (2000,
                                 [checker]
                                 {
                                     if (!checker)
                                         return;

                                     LicenseUpdaterJuce::Ptr updater;
                                     updater->fetchIfNecessary();
                                 });
}

LicenseUpdaterJuce::~LicenseUpdaterJuce()
{
    masterReference.clear();
}

void LicenseUpdaterJuce::setupLicenseData (const std::string& licenseFileName, std::string_view hwUID, std::initializer_list<std::pair<std::string, std::string>> dataToUse)
{
    licenseFile = juce::File (licenseFileName.data());
    hardwareUid = hwUID.data();
    data        = dataToUse;
}

bool LicenseUpdaterJuce::setServerResponse (juce::String plain)
{
    auto json = juce::JSON::parse (plain);

    lastErrorString = json.getProperty (LicenseID::error, "").toString().toStdString();
    if (json.hasProperty (LicenseID::checked))
        lastCheck = Helpers::decodeDateTime (json.getProperty (LicenseID::checked, "1970-01-01 00:00:00").toString().toStdString(), "%Y-%m-%d %H:%M:%S");

    if (json.getProperty (LicenseID::hardware, "").toString() != hardwareUid)
    {
        lastError = LicenseDefines::Error::HardwareMismatch;
        sendChangeMessage();
        return false;
    }

    sendChangeMessage();
    return true;
}

juce::String LicenseUpdaterJuce::getLicenseText() const
{
    const juce::ScopedLock lock (licenseFileLock);
    if (auto stream = licenseFile.createInputStream())
    {
        auto cipher = stream->readEntireStreamAsString();
        auto text   = Crypto::decrypt (cipher.toStdString());

        return text;
    }
    return {};
}

void LicenseUpdaterJuce::fetchIfNecessary (int hours)
{
    auto plain = getLicenseText();
    if (plain.isNotEmpty())
    {
        setServerResponse (plain);
    }

    auto now = time (nullptr);
    if (lastCheck && (now - *lastCheck) < 3600 * hours)
    {
        sendChangeMessage();
        return;
    }
    fetchLicenseData();
}

void LicenseUpdaterJuce::fetchLicenseData (std::string_view action, const std::vector<std::pair<std::string, std::string>>& additionalData)
{
    juce::var json (new juce::DynamicObject);
    if (auto* object = json.getDynamicObject())
    {
        object->setProperty (LicenseID::action, action.data());
        object->setProperty (LicenseID::product, LicenseData::productUid);

        for (const auto& item: data)
            object->setProperty (item.first.data(), item.second.data());

        for (const auto& item: additionalData)
            object->setProperty (item.first.data(), item.second.data());
    }

    auto      payload = juce::JSON::toString (json);
    juce::URL url (juce::String (LicenseData::authServerUrl) + "auth/");

    url = url.withPOSTData (payload);

    downloadTask = url.downloadToFile (tempFile.getFile(), juce::URL::DownloadTaskOptions().withUsePost (true).withListener (this));
}

void LicenseUpdaterJuce::finished (juce::URL::DownloadTask* task, bool success)
{
    if (success)
    {
        task->getTargetLocation();
        auto result = task->getTargetLocation().loadFileAsString();

        if (result.isNotEmpty())
        {
            auto plain = Crypto::decrypt (result.toStdString());

            if (setServerResponse (plain))
            {
                task->getTargetLocation().moveFileTo (licenseFile);
            }
        }
        else
        {
            lastError = LicenseDefines::Error::ServerAnswerInvalid;
        }
    }

    task->getTargetLocation().deleteFile();
    sendChangeMessage();
}


}  // namespace foleys