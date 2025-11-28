/********************************************************************************
    Copyright 2024 - Daniel Walz
    Foleys Finest Audio UG (haftungsbeschraenkt)
    Lobith Audio UG (haftungsbeschraenkt)
 ********************************************************************************

    This code is provided under the ISC license

    This code is provided as is. The authors disclaim all warranties.
    For details refer to the LICENSE.md

 *******************************************************************************/


#ifndef FOLEYS_LICENSE_CLIENT_FOLEYS_LICENSE_H
#define FOLEYS_LICENSE_CLIENT_FOLEYS_LICENSE_H

#include "foleys_LicenseData.h"
#include "foleys_LicenseDefines.h"
#include "foleys_LicenseHelpers.h"

#ifndef FOLEYS_LICENSE_ENCRYPT_REQUEST
    #define FOLEYS_LICENSE_ENCRYPT_REQUEST 0
#endif

namespace foleys
{

class License
{
public:
    License();
    ~License();

    enum State
    {
        Unknown = 0,
        Error,
        DemoExpired,
        Expired,
        DemoAvailable,
        DemoRunning,
        ActivationsUsed,
        ActivationsAvailable,
        Activated
    };

    /**
     * Check if a popup should be shown.
     * Usually when there is no local license and in demo only once per day.
     * @return
     */
    [[nodiscard]] bool shouldShowPopup();

    /**
     * Access the server for a new state
     */
    void syncLicense();

    /**
     * Check the last error code from the license server or from failed attempts to synchronise
     * @return
     */
    [[nodiscard]] LicenseDefines::Error getLastError() const;

    /**
     * If the last action resulted in an error, this will return it
     * @return the error as string
     */
    [[nodiscard]] std::string getLastErrorString() const;

    /**
     * Check if the machine is activated.
     * This value is cached to be fast and thread safe, but less secure.
     *
     * @return true if this machine is checked and activated.
     */
    [[nodiscard]] bool isActivated() const;

    /**
     * Check if the machine is expired (according to server time)
     * This value is cached to be fast and thread safe, but less secure.
     *
     * @return true if the expiry date is in the past.
     */
    [[nodiscard]] bool isExpired() const;

    /**
     * This is a shorthand that either a license that is not expired or a demo is running
     * @return true if is allowed to run
     */
    [[nodiscard]] bool isAllowed() const;

    /**
     * This returns the expiry time/date or nullopt if it is a perpetual license.
     * @return The expiry date
     */
    std::optional<std::time_t> expires() const;

    /**
     * @return the timestamp when the last server response was received
     */
    std::optional<std::time_t> lastChecked() const;

    /**
     * @return true if the last request was activate
     */
    bool lastActionWasActivate() const;

    /**
     * When activated this can be used to display the licensee
     * @return the email the license is licensed to
     */
    [[nodiscard]] std::string getLicenseeEmail() const;

    /**
     * Send a login request. The server will send an email link to the supplied email.
     */
    void sendLoginEmail (const std::string& email);

    /**
     * Request the server to activate this computer
     * @param data is a name/value set to add to the request payload.
     */
    void activate (const std::vector<std::pair<std::string, std::string>>& data);

    /**
     * Deactivate another machine using the activation ID
     * @param data is a name/value set to add to the request payload.
     */
    void deactivate (size_t otherID, const std::vector<std::pair<std::string, std::string>>& data = {});

    /**
     * In case of a failed activation, this contains existing activations for deactivation
     * @return a list of activated machines
     */
    std::vector<Activation> getActivations() const;

    /**
     * Check if the user is allowed to start a demo
     * @return true, if a demo is available. False if the product doesn't offer one or the demo was already used.
     */
    [[nodiscard]] bool canDemo() const;

    /**
     * @return True, if the product is currently running as demo.
     */
    [[nodiscard]] bool isDemo() const;

    /**
     * @return the number of days left to demo. Will be negative if the demo expired.
     */
    [[nodiscard]] int demoDaysLeft() const;

    /**
     * @return the end date of the demo for display. Empty string if demo was never started.
     */
    [[nodiscard]] std::optional<std::time_t> getDemoEndDate() const;

    /**
     * Send a demo request to the server
     */
    void startDemo();

    /**
     * Set a flag in the updater to avoid multiple times popup
     * @param wasShown
     */
    void setPopupWasShown (bool wasShown);

    /**
     * This is called when a valid license was received
     */
    std::function<void()> onLicenseReceived;

    /**
     * Set the path for the license file. This needs to be called only once
     * @param licenseFile the filename to store the license to (no std::filesystem before 10.15)
     * @param hwUID the hardware UID of the running system
     * @param data a string pair vector of data to send with every payload
     */
    void setupLicenseData (const FF_PATH & licenseFile, std::string_view hwUID, std::initializer_list<std::pair<std::string, std::string>> data = {});

    /**
     * Try to set the content of a license file and store it on success
     * @param content the encrypted content of the file
     * @return true if applied successfully
     */
    bool setOfflineLicenseData (std::string_view content);

    /**
     * Create a payload for an activation request
     * @return
     */
    [[nodiscard]] std::string getOfflineRequest() const;

    /**
     * Callback for the LicenseUpdater
     */
    void licenseChanged();

    /**
     * This is for debugging only. Don't use this in production.
     * @return the raw data received from the server
     */
    std::string getRawLicenseData() const;


    /**
     * @return a state describing the current license state
     */
    [[nodiscard]] State getState() const;

private:
    void syncPimpl();

    std::atomic<bool> activatedFlag = false;
    std::atomic<bool> demoAvailable = false;
    std::atomic<bool> allowedFlag   = false;
    std::atomic<int>  demoDays      = 0;


    struct Pimpl;
    std::unique_ptr<Pimpl> pimpl;
};


}  // namespace foleys

#endif  // FOLEYS_LICENSE_CLIENT_FOLEYS_LICENSE_H
