//
// Created by Daniel Walz on 10.03.25.
//

#include "foleys_license_common/foleys_LicenseHelpers.h"

#include "foleys_SharedObject.h"
#include "foleys_LicenseUpdater.h"

#include <nlohmann/json.hpp>

namespace foleys
{

struct License::Pimpl : public LicenseUpdater::Observer
{
    explicit Pimpl (License& ownerToUse) : owner (ownerToUse)
    {
        updater->addObserver (this);

        auto text = updater->getLicenseText();
        if (!text.empty())
            setLicenseData (text);
    }

    ~Pimpl() override { updater->removeObserver (this); }

    void setupLicenseData (const std::string& licenseFile, std::string_view hwUID, std::initializer_list<std::pair<std::string, std::string>> data)
    {
        updater->setupLicenseData (licenseFile, hwUID, data);
    }


    [[nodiscard]] std::string getLicenseeEmail() const { return email; }

    void fetchLicenseData (std::string_view action = {}, const std::vector<std::pair<std::string, std::string>>& data = {})
    {
        updater->fetchLicenseData (action, data);
    }

    [[nodiscard]] bool isDemo() const { return demoDays.load() > 0 && !owner.demoAvailable; }

    bool shouldShowPopup() { return !owner.isAllowed() || (!updater->popupWasShown() && !owner.activatedFlag.load()); }

    void setPopupWasShown (bool wasShown) { updater->setPopupWasShown (wasShown); }

    [[nodiscard]] bool isExpired() const
    {
        if (expiryDate)
            return *expiryDate < std::time (nullptr);

        return false;
    }

    [[nodiscard]] LicenseDefines::Error getLastError() const { return updater->getLastError(); }

    [[nodiscard]] std::string getLastErrorString() const { return updater->getLastErrorString(); }

    [[nodiscard]] std::string getOfflineRequest() const { return updater->getOfflineRequest(); }

    [[nodiscard]] std::vector<Activation> getActivations() { return {}; }

    void licenseUpdated() override
    {
        auto text = updater->getLicenseText();
        if (!text.empty())
            setLicenseData (text);

        owner.licenseChanged();
    }

    bool setOfflineLicenseData (std::string_view content) { return false; }

    std::pair<LicenseDefines::Error, std::string> setLicenseData (std::string_view text)
    {
        auto json = nlohmann::json::parse (updater->getLicenseText(), nullptr, false);
        if (json.is_discarded())
        {
            owner.activatedFlag = false;
            owner.allowedFlag   = false;
            return { LicenseDefines::Error::ServerAnswerInvalid, "Got invalid license data (bad json)" };
        }
        checked             = Helpers::decodeDateTime (json[LicenseID::checked], "%Y-%m-%d %H:%M:%S");
        owner.activatedFlag = json[LicenseID::activated];
        email               = json[LicenseID::licensee_email];

        licenseHardware = json[LicenseID::hardware];

        if (json.contains (LicenseID::license_expires))
            expiryDate = Helpers::decodeDateTime (json[LicenseID::license_expires], "%Y-%m-%d");
        else
            expiryDate = std::nullopt;

        if (json.contains (LicenseID::demo_available))
        {
            owner.demoAvailable = json[LicenseID::demo_available];
            demoDays            = json[LicenseID::demo_days];
            if (json.contains (LicenseID::demo_ends))
            {
                auto ends          = Helpers::decodeDateTime (json[LicenseID::demo_ends], "%Y-%m-%d");
                auto localDemoDays = static_cast<int> (1 + difftime (ends, std::time (nullptr)) / (24 * 3600));
                demoDays           = std::min (demoDays.load(), localDemoDays);
            }
        }
        else
        {
            owner.demoAvailable = false;
            demoDays            = 0;
        }

        if (json.contains (LicenseID::error))
        {
            owner.activatedFlag = false;
            owner.allowedFlag   = false;
            return { LicenseDefines::Error::ServerError, json[LicenseID::error] };
        }

        owner.allowedFlag = (owner.activatedFlag && !isExpired()) || isDemo();

        return { LicenseDefines::Error::NoError, {} };
    }

    std::string getRawLicenseData() const
    {
        return updater->getLicenseText();
    }


    SharedObject<LicenseUpdater> updater;
    License&                     owner;
    std::mutex                   processLock;
    std::string                  licenseHardware;
    std::string                  email;
    std::atomic<int>             demoDays = 0;
    std::optional<std::time_t>   expiryDate;
    std::optional<std::time_t>   checked;

    Pimpl (const Pimpl&)            = delete;
    Pimpl (const Pimpl&&)           = delete;
    Pimpl& operator= (const Pimpl&) = delete;
};


}  // namespace foleys
