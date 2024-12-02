/********************************************************************************
Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************/


#ifndef FOLEYS_LICENSING_CLIENT_LICENSEPANEL_H
#define FOLEYS_LICENSING_CLIENT_LICENSEPANEL_H

#include "juce/foleys_PopupHolder.h"

#include <juce_gui_basics/juce_gui_basics.h>
#include <foleys_License.h>


class LicensePanel : public foleys::Popup
{
public:
    explicit LicensePanel();

    void paint (juce::Graphics& g) override;
    void resized() override;

    void update();

    void activate (const juce::String& serial, size_t deactivate);

private:
    foleys::License license;

    juce::Label          title;
    juce::Label          enterSerial { {}, TRANS ("ENTER SERIAL") };
    juce::TextEditor     code;
    juce::TextButton     submit { TRANS ("ENTER"), TRANS ("Submit code") };
    juce::Label          status;
    juce::TextButton     demo { TRANS ("Start Demo"), TRANS ("Start your 14 days free trial period") };
    juce::TextButton     deactivate { TRANS ("Deactivate"), TRANS ("Deactivate this machine") };
    juce::DrawableButton closeButton { "Close Panel", juce::DrawableButton::ButtonStyle::ImageFitted };
    juce::DrawableButton refreshButton { "Refresh license", juce::DrawableButton::ButtonStyle::ImageFitted };
    juce::DrawableButton manualButton { "Manual Guide", juce::DrawableButton::ButtonStyle::ImageAboveTextLabel };
    juce::DrawableButton homeButton { "My Licenses", juce::DrawableButton::ButtonStyle::ImageAboveTextLabel };
    juce::DrawableButton websiteButton { "Buy License", juce::DrawableButton::ButtonStyle::ImageAboveTextLabel };
    juce::Label          copyright;

    std::unique_ptr<juce::Component> deactivationPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LicensePanel)
};


#endif  // FOLEYS_LICENSING_CLIENT_LICENSEPANEL_H
