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

    void setupLicenseData (const std::string& licenseFile, std::string_view hwUID, std::initializer_list<std::pair<std::string, std::string>> data)
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

    [[nodiscard]] bool isDemo() const { return demoDays > 0 && !owner.demoAvailable; }

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


    [[nodiscard]] std::vector<Activation> getActivations()
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
                                                item.getProperty (LicenseID::computer, "").toString().toStdString(),
                                                item.getProperty (LicenseID::user, "").toString().toStdString() };
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

    std::pair<LicenseDefines::Error, std::string> setLicenseData (const juce::String& text)
    {
        auto data = juce::JSON::parse (text);

        if (auto* object = data.getDynamicObject())
        {
            checked             = Helpers::decodeDateTime (object->getProperty (LicenseID::checked).toString().toStdString(), "%Y-%m-%d %H:%M:%S");
            owner.activatedFlag = object->getProperty (LicenseID::activated);
            email               = object->getProperty (LicenseID::licensee_email).toString().toStdString();

            licenseHardware = object->getProperty (LicenseID::hardware).toString().toStdString();

            if (object->hasProperty (LicenseID::license_expires))
                expiryDate = Helpers::decodeDateTime (object->getProperty (LicenseID::license_expires).toString().toStdString(), "%Y-%m-%d");
            else
                expiryDate = std::nullopt;

            if (object->hasProperty (LicenseID::demo_available))
            {
                owner.demoAvailable = object->getProperty (LicenseID::demo_available);
                demoDays            = object->getProperty (LicenseID::demo_days);
                if (object->hasProperty (LicenseID::demo_ends))
                {
                    auto ends          = Helpers::decodeDateTime (object->getProperty (LicenseID::demo_ends).toString().toStdString(), "%Y-%m-%d");
                    auto localDemoDays = static_cast<int> (1 + difftime (ends, std::time (nullptr)) / (24 * 3600));
                    demoDays           = std::min (demoDays.load(), localDemoDays);
                }
            }
            else
            {
                owner.demoAvailable = false;
                demoDays            = 0;
            }

            if (object->hasProperty (LicenseID::error))
            {
                owner.allowedFlag = false;
                return { LicenseDefines::Error::ServerError, object->getProperty (LicenseID::error).toString().toStdString() };
            }

            owner.allowedFlag = (owner.activatedFlag && !isExpired()) || isDemo();

            return { LicenseDefines::Error::NoError, {} };
        }

        owner.allowedFlag   = false;
        owner.activatedFlag = false;

        return { LicenseDefines::Error::ServerAnswerInvalid, "Got invalid license data (bad json)" };
    }


    juce::SharedResourcePointer<foleys::LicenseUpdaterJuce> updater;
    License&                                                owner;
    juce::CriticalSection                                   processLock;
    std::string                                             licenseHardware;
    std::string                                             email;
    std::atomic<int>                                        demoDays = 0;
    std::optional<std::time_t>                              expiryDate;
    std::optional<std::time_t>                              checked;

    Pimpl (const Pimpl&)            = delete;
    Pimpl (const Pimpl&&)           = delete;
    Pimpl& operator= (const Pimpl&) = delete;
};


}  // namespace foleys
