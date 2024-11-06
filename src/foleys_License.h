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

#include "private/foleys_SharedObject.h"
#include "private/foleys_Observers.h"
#include "private/foleys_LicenseData.h"
#include "private/foleys_LicenseUpdater.h"

#include <atomic>
#include <optional>
#include <ctime>
#include <filesystem>


#ifndef FOLEYS_LICENSE_ENCRYPT_REQUEST
    #define FOLEYS_LICENSE_ENCRYPT_REQUEST 0
#endif

namespace foleys
{

class License : private LicenseUpdater::Observer
{
public:
    License();
    ~License() override;

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


    [[nodiscard]] Licensing::Error getLastError() const;

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
     * When activated this can be used to display the licensee
     * @return the email the license is licensed to
     */
    [[nodiscard]] std::string getLicenseeEmail() const;

    /**
     * Send a login request. The server will send a login link to the supplied email.
     */
    void login (const std::string& email);

    /**
     * Request the server to activate this computer
     * @param data is a name/value set to add to the request payload.
     */
    void activate (const std::vector<std::pair<std::string, std::string>>& data);

    /**
     * Deactivate this machine using the hardware UID
     * @param data is a name/value set to add to the request payload.
     */
    void deactivate (const std::vector<std::pair<std::string, std::string>>& data = {});

    /**
     * In case of a failed activation, this contains existing activations for deactivation
     * @return a list of activated machines
     */
    std::vector<Licensing::Activation> getActivations();

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
     * Send a demo request to the server
     */
    void startDemo();

    /**
     * Set a flag in the updater to avoid multiple times popup
     * @param wasShown
     */
    void setPopupWasShown (bool wasShown) { updater->setPopupWasShown (wasShown); }

    /**
     * This is called when a valid license was received
     */
    std::function<void()> onLicenseReceived;

    /**
     * Set the path for the license file. This needs to be called only once
     * @param licenseFile the filename to store the license to
     * @param hwUID the hardware UID of the running system
     * @param data a string pair vector of data to send with every payload
     */
    void setupLicenseData (const std::filesystem::path& licenseFile, std::string_view hwUID, std::initializer_list<std::pair<std::string, std::string>> data = {});

    [[nodiscard]] static time_t decodeDateTime (const std::string& timeString, const char* formatString);

private:
    std::pair<Licensing::Error, std::string> loadLicenseBlob();

    void licenseUpdated() override;

    [[nodiscard]] std::pair<Licensing::Error, std::string> processData (std::string_view data);

    LicenseUpdater::Ptr updater;

    mutable std::mutex processingLock;

    std::string                licenseHardware;
    std::string                email;
    std::atomic<bool>          activatedFlag = false;
    std::atomic<bool>          demoAvailable = false;
    std::atomic<int>           demoDays      = 0;
    std::optional<std::time_t> expiryDate;
    std::time_t                checked = {};
};


}  // namespace foleys

#endif  // FOLEYS_LICENSE_CLIENT_FOLEYS_LICENSE_H
