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

    explicit LicensePanel (bool embed = true);

    void addLinkButton (std::unique_ptr<juce::Button>&& newButton);

    void initialize();

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
    bool                  m_embedded = false;
    foleys::License       m_license;
    juce::TabbedComponent m_actionTabs { juce::TabbedButtonBar::TabsAtTop };
    juce::Label           m_title;
    juce::Label           m_status;
    juce::DrawableButton  m_closeButton { "Close Panel", juce::DrawableButton::ButtonStyle::ImageFitted };
    juce::Label           m_copyright;
    juce::Label           m_timestamp;

    std::unique_ptr<juce::Component>           m_deactivationPanel;
    std::vector<std::unique_ptr<juce::Button>> m_linkButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LicensePanel)

public:
    std::unique_ptr<juce::Drawable> m_offlineIcon;
    juce::DrawableButton            m_refreshButton { "Refresh license", juce::DrawableButton::ButtonStyle::ImageFitted };
};

}  // namespace foleys

#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEPANEL_H
