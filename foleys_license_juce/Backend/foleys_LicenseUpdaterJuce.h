//
// Created by Daniel Walz on 19.02.25.
//

#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEUPDATERJUCE_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEUPDATERJUCE_H


#include "../../foleys_license_common/foleys_LicenseDefines.h"

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

namespace foleys
{

class LicenseUpdaterJuce
  : public juce::ChangeBroadcaster
  , public juce::URL::DownloadTaskListener
{
public:
    LicenseUpdaterJuce();
    ~LicenseUpdaterJuce() override;

    /**
     * Setup the license client
     * @param licenseFile the location where to save and load the license to
     * @param hwUID a unique hardware uid of the machine trying to authenticate
     * @param data an associative array of data you want to include in all requests
     */
    void setupLicenseData (const std::string& licenseFile, std::string_view hwUID, std::initializer_list<std::pair<std::string, std::string>> data);

    /**
     * Process the license data for the updater
     * @param plain the license decrypted
     * @returns true if the license was valid and applicable
     */
    bool setServerResponse (juce::String plain);

    /**
     * Allow all License objects to read the license content.
     * This is thread safe through a mutex
     * @return the license content in plain text
     */
    juce::String getLicenseText() const;

    /**
     * If the last action resulted in an error, this will return it
     * @return the error as string
     */
    [[nodiscard]] LicenseDefines::Error getLastError() const { return lastError; }

    /**
     * If the last action resulted in an error, this will return it
     * @return the error as string
     */
    [[nodiscard]] std::string getLastErrorString() const { return lastErrorString; }


    /**
     * This method checks if a license file is present and if it was fetched less than 24 hours before.
     * Otherwise it will fetch a new license
     */
    void fetchIfNecessary (int hours = 24);

    /**
     * Tries to get new license data from the server.
     * @param action an optional action. Allowed values: 'demo' or 'activate'. Anything else just gets the status
     */
    void fetchLicenseData (std::string_view action = {}, const std::vector<std::pair<std::string, std::string>>& data = {});

    /**
     * Callback from the license download
     * @param task
     * @param success
     */
    void finished (juce::URL::DownloadTask* task, bool success) override;

    /**
     * Create a payload for an offline request
     * @return
     */
    [[nodiscard]] std::string getOfflineRequest() const;

    /**
     * Try and store license file
     * @param content
     * @return true if the license was accepted
     */
    bool setOfflineLicenseData (std::string_view content);

    /**
     * Check a local flag, if the popup was already dismissed by the user (is reset on restart)
     * @return
     */
    bool popupWasShown() const { return popupShown; }

    /**
     * Set the flag when the popup was already shown to the user
     * @param wasShown
     */
    void setPopupWasShown (bool wasShown) { popupShown.store (wasShown); }

private:
    using Ptr = juce::SharedResourcePointer<LicenseUpdaterJuce>;
    JUCE_DECLARE_WEAK_REFERENCEABLE (LicenseUpdaterJuce)

    std::unique_ptr<juce::URL::DownloadTask>         downloadTask;
    juce::File                                       licenseFile;
    juce::CriticalSection                            licenseFileLock;
    juce::TemporaryFile                              tempFile;
    juce::String                                     hardwareUid;
    std::vector<std::pair<std::string, std::string>> data;

    LicenseDefines::Error lastError = LicenseDefines::Error::NoError;
    std::string           lastErrorString;

    std::atomic<bool>     popupShown { false };
    std::optional<time_t> lastCheck;
};


}  // namespace foleys


#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEUPDATERJUCE_H
