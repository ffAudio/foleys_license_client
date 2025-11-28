//
// Created by Daniel Walz on 21.02.25.
//


#include "foleys_license_common/foleys_License.h"
#include "foleys_license_common/foleys_LicenseHelpers.h"
#include "foleys_LicenseUpdaterJuce.h"

#include <juce_events/juce_events.h>

namespace foleys
{

struct License::Pimpl : public juce::ChangeListener
{
    explicit Pimpl (License& ownerToUse) : owner (ownerToUse)
    {
        updater->addChangeListener (this);
        reload();
    }

    ~Pimpl() override { updater->removeChangeListener (this); }

    void setupLicenseData (const FF_PATH& licenseFile, std::string_view hwUID, std::initializer_list<std::pair<std::string, std::string>> data)
    {
        updater->setupLicenseData (licenseFile, hwUID, data);
    }

    void reload()
    {
        auto text = updater->getLicenseText();
        if (text.isNotEmpty())
            setLicenseData (text);
    }

    [[nodiscard]] std::string getLicenseeEmail() const { return email; }

    void fetchLicenseData (std::string_view action = {}, const std::vector<std::pair<std::string, std::string>>& data = {})
    {
        updater->fetchLicenseData (action, data);
    }

    bool shouldShowPopup() { return !owner.isAllowed() || (!updater->popupWasShown() && !activatedFlag.load()); }

    void setPopupWasShown (bool wasShown) { updater->setPopupWasShown (wasShown); }

    [[nodiscard]] bool isExpired() const
    {
        if (expiryDate)
            return *expiryDate < std::time (nullptr);

        return false;
    }

    [[nodiscard]] LicenseDefines::Error getLastError() const { return updater->getLastError(); }

    [[nodiscard]] std::string getLastErrorString() const { return updater->getLastErrorString(); }

    [[nodiscard]] std::vector<Activation> getActivations() const
    {
        auto text = updater->getLicenseText();

        auto data = juce::JSON::parse (text);
        if (auto* object = data.getDynamicObject())
        {
            if (object->hasProperty (LicenseID::activations))
            {
                if (auto* array = object->getProperty (LicenseID::activations).getArray())
                {
                    std::vector<Activation> acts;
                    for (const auto& item: *array)
                    {
                        Activation activation { static_cast<size_t> (int (item.getProperty (LicenseID::id, 0))),
                                                item.getProperty (LicenseID::computer, "").toString().toRawUTF8(),
                                                item.getProperty (LicenseID::user, "").toString().toRawUTF8() };
                        acts.push_back (activation);
                    }
                    return acts;
                }
            }
        }

        return {};
    }

    void changeListenerCallback (juce::ChangeBroadcaster*) override
    {
        reload();
        owner.licenseChanged();
    }

    [[nodiscard]] std::string getOfflineRequest() const { return updater->getOfflineRequest(); }

    bool setOfflineLicenseData (std::string_view content) { return updater->setOfflineLicenseData (content); }

    std::pair<LicenseDefines::Error, std::string> setLicenseData (const juce::String& text)
    {
        auto data = juce::JSON::parse (text);

        if (auto* object = data.getDynamicObject())
        {
            checked       = Helpers::decodeDateTime (object->getProperty (LicenseID::checked).toString().toRawUTF8(), "%Y-%m-%d %H:%M:%S");
            activatedFlag = object->getProperty (LicenseID::activated);
            email         = object->getProperty (LicenseID::licensee_email).toString().toRawUTF8();

            licenseHardware = object->getProperty (LicenseID::hardware).toString().toRawUTF8();


            if (object->hasProperty (LicenseID::action))
                lastActionWasActivate = object->getProperty(LicenseID::action).toString().equalsIgnoreCase(LicenseID::activate);
            else
                lastActionWasActivate = false;


            if (object->hasProperty (LicenseID::license_expires))
                expiryDate = Helpers::decodeDateTime (object->getProperty (LicenseID::license_expires).toString().toRawUTF8(), "%Y-%m-%d");
            else
                expiryDate = std::nullopt;


            if (object->hasProperty (LicenseID::demo_available))
            {
                demoAvailable = object->getProperty (LicenseID::demo_available);
                demoDays      = object->getProperty (LicenseID::demo_days);
                if (object->hasProperty (LicenseID::demo_ends))
                {
                    demoEndDate        = Helpers::decodeDateTime (object->getProperty (LicenseID::demo_ends).toString().toRawUTF8(), "%Y-%m-%d");
                    auto localDemoDays = static_cast<int> (1 + difftime (*demoEndDate, std::time (nullptr)) / (24 * 3600));
                    demoDays           = std::min (demoDays.load(), localDemoDays);
                }
                else
                {
                    demoEndDate = {};
                }
            }
            else
            {
                demoAvailable = false;
                demoDays      = 0;
            }

            if (object->hasProperty (LicenseID::error))
            {
                return { LicenseDefines::Error::ServerError, object->getProperty (LicenseID::error).toString().toRawUTF8() };
            }

            return { LicenseDefines::Error::NoError, {} };
        }

        activatedFlag = false;

        return { LicenseDefines::Error::ServerAnswerInvalid, "Got invalid license data (bad json)" };
    }

    std::string getRawLicenseData() const { return updater->getLicenseText().toRawUTF8(); }

    juce::SharedResourcePointer<foleys::LicenseUpdaterJuce> updater;
    License&                                                owner;
    juce::CriticalSection                                   processLock;
    std::string                                             licenseHardware;
    std::string                                             email;
    std::optional<std::time_t>                              demoEndDate;
    std::optional<std::time_t>                              expiryDate;
    std::optional<std::time_t>                              checked;
    std::atomic<bool>                                       activatedFlag         = false;
    std::atomic<bool>                                       demoAvailable         = false;
    std::atomic<int>                                        demoDays              = 0;
    std::atomic<bool>                                       lastActionWasActivate = false;

    Pimpl (const Pimpl&)            = delete;
    Pimpl (const Pimpl&&)           = delete;
    Pimpl& operator= (const Pimpl&) = delete;
};


}  // namespace foleys
