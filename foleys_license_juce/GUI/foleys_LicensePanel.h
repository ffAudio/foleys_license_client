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
    static constexpr auto kGrid            = 5;
    static constexpr auto kMaxRowHeight    = kGrid * 6;
    static constexpr auto kMaxWidth        = 400;
    static constexpr auto kSmallFontHeight = 12.0f;
    static constexpr auto kFontHeight      = 16.0f;
    static constexpr auto kTitleFontHeight = 24.0f;

    juce::Colour backgroundColour { 0x00000000 };
    juce::Colour textColour { 0xffc0c0c0 };
    juce::Colour accentColour { 0xff6a5acd };
    juce::Colour buttonColour { 0xffc0c0c0 };
    juce::Colour buttonBackgroundColour { 0xff6a5acd };
    juce::Colour editorBackgroundColour { 0xff708090 };

    enum class Tab
    {
        Demo = 0,
        Activation,
        Offline
    };

    struct TabButtonLookAndFeel : juce::LookAndFeel_V4
    {
        void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    };

    explicit LicensePanel (bool embed = true);
    ~LicensePanel() override;

    void addLinkButton (std::unique_ptr<juce::Button>&& newButton);

    void setTab (Tab currentTab);

    void paint (juce::Graphics& g) override;
    void resized() override;

    void update();

    bool isInterestedInFileDrag (const juce::StringArray& files) override;

    void filesDropped (const juce::StringArray& files, int x, int y) override;

    void requestClose();

    /**
     * A lambda you can customize for your own layout
     */
    std::function<void (juce::Rectangle<int>)> onResized;

    /**
     * A lambda you can customize for your own drawing
     */
    std::function<void (juce::Graphics&)> onPaint;

    /**
     * A lambda that is called when the license has changed
     */
    std::function<void (foleys::License&)> onLicenseChanged;

    enum ColourIds
    {
        buttonColourId       = 0x5a3c500,
        footerColourId       = 0x5a3c501,
        inactiveTextColourId = 0x5a3c502
    };

private:
    bool                             m_embedded = false;
    foleys::License                  m_license;
    std::unique_ptr<juce::Component> m_currentTab;
    TabButtonLookAndFeel             m_tabButtonLookAndFeel;
    juce::TextButton                 m_demoTabButton { TRANS ("Demo") };
    juce::TextButton                 m_activationTabButton { TRANS ("Activation") };
    juce::TextButton                 m_offlineTabButton { TRANS ("Offline") };
    juce::Label                      m_title;
    juce::Label                      m_status;
    juce::DrawableButton             m_closeButton { "Close Panel", juce::DrawableButton::ButtonStyle::ImageFitted };
    juce::Label                      m_copyright;
    juce::Label                      m_timestamp;

    std::unique_ptr<juce::Component>           m_deactivationPanel;
    std::vector<std::unique_ptr<juce::Button>> m_linkButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LicensePanel)

public:
    std::unique_ptr<juce::Drawable> m_offlineIcon;
    juce::DrawableButton            m_refreshButton { "Refresh license", juce::DrawableButton::ButtonStyle::ImageFitted };
};

}  // namespace foleys

#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEPANEL_H
