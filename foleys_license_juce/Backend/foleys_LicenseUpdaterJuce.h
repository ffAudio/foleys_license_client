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

    void setupLicenseData (const std::string& licenseFile, std::string_view hwUID, std::initializer_list<std::pair<std::string, std::string>> data);

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

    void finished (juce::URL::DownloadTask* task, bool success) override;

    bool popupWasShown() const { return popupShown; }
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
