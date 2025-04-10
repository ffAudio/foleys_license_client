/********************************************************************************
Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************/


#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEUPDATER_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEUPDATER_H

#include "foleys_license_common/foleys_LicenseDefines.h"
#include "foleys_Observers.h"
#include "foleys_SharedObject.h"
#include "foleys_NetworkRequest.h"


namespace foleys
{

/**
 * The LicenseUpdater is a shared instance from all License classes that handles
 * fetching new licenses to avoid duplicate requests.
 */
class LicenseUpdater
{
public:
    LicenseUpdater();

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
     * Each License is an observer to the LicenseUpdater. You can add anything as observer,
     * but more convenient just create a License object as member, which has a std::function for convenience.
     * This has also the advantage to spread the license check logic to different places in the binary.
     */
    struct Observer
    {
        virtual void licenseUpdated() = 0;

        FOLEYS_DECLARE_SAFEPOINTER (Observer)
    };

    /**
     * Register an observer when a new license answer is received or loaded. Make sure to removeObserver before you destroy the observer.
     * @param observer
     */
    void addObserver (Observer* observer);

    /**
     * Deregister an observer
     * @param observer
     */
    void removeObserver (Observer* observer);

    void setupLicenseData (const std::string& licenseFile, std::string_view hwUID, std::initializer_list<std::pair<std::string, std::string>> data);

    [[nodiscard]] std::string getLicenseText();

    /**
     * In case of a failed activation, this contains existing activations for deactivation
     * @return a list of activated machines
     */
    std::vector<Activation> getActivations();

    /**
     * Store a flag if the popup was already shown in that session
     * @return false if the popup wasn't shown yet
     */
    [[nodiscard]] bool popupWasShown() const { return popupShown; }

    void setPopupWasShown (bool wasShown) { popupShown = wasShown; }

    using Ptr                              = SharedObject<LicenseUpdater>;
    LicenseUpdater (const LicenseUpdater&) = delete;

private:
    void sendUpdateSignal();

    std::mutex  storageMutex;
    std::string hardwareUid;
    std::string localStorage;

    std::vector<std::pair<std::string, std::string>> defaultData;
    LicenseDefines::Error                            lastError = LicenseDefines::Error::NoError;
    std::string                                      lastErrorString;
    ObserverList<Observer>                           observerList;
    bool                                             popupShown = false;

    std::unique_ptr<NetworkRequest> request;
};


}  // namespace foleys

#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEUPDATER_H
