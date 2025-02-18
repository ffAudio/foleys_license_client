//
// Created by Daniel Walz on 11.10.24.
//

#ifndef FOLEYS_LICENSING_CLIENT_LICENSEDEACTIVATE_H
#define FOLEYS_LICENSING_CLIENT_LICENSEDEACTIVATE_H

#include <foleys_license_juce/foleys_license_juce.h>

#include <juce_gui_basics/juce_gui_basics.h>

class LicenseDeactivate
  : public juce::Component
  , private juce::ListBoxModel
{
public:
    LicenseDeactivate();
    ~LicenseDeactivate() override;

    void resized() override;
    void paint (juce::Graphics& g) override;

    void setCloseFunction (std::function<void()> func) { closeButton.onClick = std::move (func); }

    int              getNumRows() override;
    juce::Component* refreshComponentForRow (int rowNumber, bool isRowSelected, juce::Component* existingComponentToUpdate) override;
    void paintListBoxItem ([[maybe_unused]] int rowNumber, [[maybe_unused]] juce::Graphics& g, [[maybe_unused]] int width, [[maybe_unused]] int height,
                           [[maybe_unused]] bool rowIsSelected) override
    {
    }

    foleys::License license;

    std::function<void (size_t deactivate)> onDeactivate;

private:
    std::vector<foleys::Licensing::Activation> activations;
    juce::ListBox                              deactivations;
    juce::TextButton                           closeButton { "Close" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LicenseDeactivate)
};


#endif  // FOLEYS_LICENSING_CLIENT_LICENSEDEACTIVATE_H
