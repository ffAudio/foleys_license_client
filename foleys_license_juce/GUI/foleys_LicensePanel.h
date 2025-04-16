/********************************************************************************
Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************/


#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEPANEL_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEPANEL_H


#include "foleys_license_juce/foleys_license_juce.h"

#include "juce_gui_basics/juce_gui_basics.h"

namespace foleys
{

class LicensePanel
  : public foleys::Popup
  , public juce::FileDragAndDropTarget
{
public:
    enum Button
    {
        Unknown = 0,
        Close,
        Refresh,
        Manual,
        UserPage,
        ProductPage,
        OfflineAuth
    };

    LicensePanel();

    void paint (juce::Graphics& g) override;
    void resized() override;

    void update();

    void activate (const juce::String& serial, size_t deactivate);

    void setButtonIcon (Button buttonType, const char* imageData, size_t imageDataSize);

    bool isInterestedInFileDrag (const juce::StringArray& files) override;

    void filesDropped (const juce::StringArray& files, int x, int y) override;

    /**
     * A lambda you can customize for your own layout
     */
    std::function<void(juce::Rectangle<int>)> onResized;

    /**
     * A lambda you can customize for your own drawing
     */
    std::function<void(juce::Graphics&)> onPaint;

    /**
     * A lambda that is called when the license has changed
     */
    std::function<void(foleys::License&)> onLicenseChanged;

private:
    foleys::License license;

    juce::Label            title;
    juce::Label            enterSerial { {}, TRANS ("ENTER SERIAL") };
    juce::TextEditor       code;
    juce::TextButton       submit { TRANS ("ENTER"), TRANS ("Submit code") };
    juce::Label            status;
    juce::TextButton       demo { TRANS ("Start Demo"), TRANS ("Start your 14 days free trial period") };
    juce::TextButton       deactivate { TRANS ("Deactivate"), TRANS ("Deactivate this machine") };
    juce::DrawableButton   closeButton { "Close Panel", juce::DrawableButton::ButtonStyle::ImageFitted };
    juce::DrawableButton   refreshButton { "Refresh license", juce::DrawableButton::ButtonStyle::ImageFitted };
    juce::DrawableButton   manualButton { "Manual Guide", juce::DrawableButton::ButtonStyle::ImageAboveTextLabel };
    juce::DrawableButton   homeButton { "My Licenses", juce::DrawableButton::ButtonStyle::ImageAboveTextLabel };
    juce::DrawableButton   websiteButton { "Buy License", juce::DrawableButton::ButtonStyle::ImageAboveTextLabel };
    foleys::FileDragButton offlineButton { "Offline Activation", juce::DrawableButton::ButtonStyle::ImageAboveTextLabel };
    juce::Label            copyright;
    juce::Label            timestamp;

    std::unique_ptr<juce::Component> deactivationPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LicensePanel)
};

}  // namespace foleys

#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEPANEL_H
