/********************************************************************************
Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************/


#include "foleys_license_juce/foleys_license_juce.h"

#include "foleys_LicensePanel.h"
#include "foleys_LicenseDeactivate.h"

namespace foleys
{

// ================================================================================

#if JUCE_VERSION >= 0x080000
    #define SET_JUCE_FONT(Component, Size) Component.setFont (juce::FontOptions { Size });
#else
    #define SET_JUCE_FONT(Component, Size) Component.setFont (juce::Font (Size));
#endif

// ================================================================================

struct DemoTab : juce::Component
{
    explicit DemoTab (LicensePanel& owner) : m_owner (owner)
    {
        addAndMakeVisible (m_demoStatus);
        m_demoStatus.setJustificationType (juce::Justification::centred);
        SET_JUCE_FONT (m_demoStatus, LicensePanel::kFontHeight)

        m_license.onLicenseReceived = [this]
        {
            auto addButton = [this] (const juce::String& title, auto function)
            {
                auto button = std::make_unique<juce::TextButton> (title);
                addAndMakeVisible (button.get());
                button->onClick = std::move (function);
                m_buttons.push_back (std::move (button));
            };

            m_buttons.clear();
            if (m_license.isActivated() && !m_license.isExpired())
            {
                m_demoStatus.setText (TRANS ("Your machine is activated, you are good to go."), juce::sendNotification);
                addButton (TRANS ("Continue"), [this] { m_owner.requestClose(); });
            }
            else if (m_license.isDemo())
            {
                m_demoStatus.setText (TRANS ("Your demo will expire on " + juce::String (m_license.getDemoEndDate())), juce::sendNotification);
                addButton (TRANS ("Continue"), [this] { m_owner.requestClose(); });
                addButton (TRANS ("Buy a license"), [] { juce::URL (LicenseData::buyUrl).launchInDefaultBrowser(); });
            }
            else if (m_license.canDemo())
            {
                m_demoStatus.setText (TRANS ("Start your free demo now for " + juce::String (m_license.demoDaysLeft()) + " days"), juce::sendNotification);
                addButton (TRANS ("Start Demo"), [this] { m_license.startDemo(); });
            }
            else  // demo expired
            {
                m_demoStatus.setText (TRANS ("Your demo expired on " + juce::String (m_license.getDemoEndDate())), juce::sendNotification);
                addButton (TRANS ("Buy a license"), [] { juce::URL (LicenseData::buyUrl).launchInDefaultBrowser(); });
            }

            resized();
        };

        m_license.onLicenseReceived();
    }

    void resized() override
    {
        m_demoStatus.setBounds (getLocalBounds().withHeight (getHeight() / 3));
        auto buttonsArea = getLocalBounds().withTop (getHeight() / 3);
        if (m_buttons.size() == 1)
        {
            m_buttons.front()->setBounds (buttonsArea.withSizeKeepingCentre (200, 36));
        }
        else if (!m_buttons.empty())
        {
            const auto w = buttonsArea.getWidth() / int (m_buttons.size());
            buttonsArea  = buttonsArea.withSizeKeepingCentre (getWidth(), 36);
            for (const auto& button: m_buttons)
                button->setBounds (buttonsArea.removeFromLeft (w));
        }
    }

    LicensePanel&                                  m_owner;
    foleys::License                                m_license;
    juce::Label                                    m_demoStatus;
    std::vector<std::unique_ptr<juce::TextButton>> m_buttons;
};

// ================================================================================

struct ActivationTab : juce::Component
{
    explicit ActivationTab (LicensePanel& owner) : m_owner (owner)
    {
        addAndMakeVisible (m_status);
        m_status.setJustificationType (juce::Justification::centred);
        SET_JUCE_FONT (m_status, LicensePanel::kFontHeight)

        m_serialEditor.setTextToShowWhenEmpty ("XXXX-YYYY-ZZZZ-WWWW", juce::Colours::grey);
        m_serialEditor.setJustification (juce::Justification::centred);

        m_submit.onClick = [this]
        {
            if (m_serialEditor.isEmpty())
                return;

            std::vector<std::pair<std::string, std::string>> data = { { LicenseID::computer, juce::SystemStats::getComputerName().toRawUTF8() },
                                                                      { LicenseID::user, juce::SystemStats::getFullUserName().toRawUTF8() },
                                                                      { LicenseID::serial, m_serialEditor.getText().toRawUTF8() } };

            m_license.activate (data);
        };

        addAndMakeVisible (m_serialEditor);
        addAndMakeVisible (m_submit);

        m_license.onLicenseReceived = [this]
        {
            auto addButton = [this] (const juce::String& title, auto function)
            {
                auto button = std::make_unique<juce::TextButton> (title);
                addAndMakeVisible (button.get());
                button->onClick = std::move (function);
                m_buttons.push_back (std::move (button));
            };

            m_buttons.clear();

            if (m_license.isActivated())
            {
                if (m_license.isExpired())
                {
                    auto date = juce::String (Helpers::formatDateTime (*m_license.expires(), "%d. %m. %Y %H:%M")) + " UTC";
                    m_status.setText ("Your license expired on " + date, juce::sendNotification);
                    addButton (TRANS ("Renew license"), [] { juce::URL (LicenseData::buyUrl).launchInDefaultBrowser(); });
                }
                else if (m_license.expires())
                {
                    auto date = juce::String (Helpers::formatDateTime (*m_license.expires(), "%d. %m. %Y %H:%M")) + " UTC";
                    m_status.setText ("Your license will expire on " + date, juce::sendNotification);
                    addButton (TRANS ("Renew license"), [] { juce::URL (LicenseData::buyUrl).launchInDefaultBrowser(); });
                    addButton (TRANS ("Close"), [this] { m_owner.requestClose(); });
                }
                else
                {
                    m_status.setText ("Licensed to ", juce::sendNotification);
                    addButton (TRANS ("Close"), [this] { m_owner.requestClose(); });
                }
            }
            else
            {
                m_status.setText ("Please enter a serial number and hit activate", juce::sendNotification);
                addButton (TRANS ("Buy a license"), [] { juce::URL (LicenseData::buyUrl).launchInDefaultBrowser(); });
                if (m_license.isDemo())
                    addButton (TRANS ("Continue Demo"), [this] { m_owner.requestClose(); });
            }

            if (!m_license.getLastErrorString().empty())
                m_status.setText (m_license.getLastErrorString(), juce::sendNotification);

            resized();
        };

        m_license.onLicenseReceived();
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        auto line1  = bounds.removeFromTop (50).reduced (0, 10);
        m_serialEditor.setBounds (line1.removeFromLeft (bounds.getWidth() - 200));
        m_submit.setBounds (line1.withTrimmedLeft (10));
        auto line2 = bounds.removeFromTop (50).reduced (0, 10);
        m_status.setBounds (line2);

        if (!m_buttons.empty())
        {
            const auto w = bounds.getWidth() / int (m_buttons.size());
            bounds       = bounds.withSizeKeepingCentre (getWidth(), 36);
            for (const auto& button: m_buttons)
                button->setBounds (bounds.removeFromLeft (w));
        }
    }

    LicensePanel&                                  m_owner;
    foleys::License                                m_license;
    juce::TextEditor                               m_serialEditor;
    juce::TextButton                               m_submit { TRANS ("Activate") };
    juce::Label                                    m_status;
    std::vector<std::unique_ptr<juce::TextButton>> m_buttons;
};

// ================================================================================

struct OfflineTab : juce::Component
{
    explicit OfflineTab (LicensePanel& owner) : m_owner (owner)
    {
        addAndMakeVisible (m_offlineButton);
        addAndMakeVisible (m_status);
        m_status.setJustificationType (juce::Justification::centred);
        m_offlineButton.setImages (m_owner.m_offlineIcon.get());
        SET_JUCE_FONT (m_status, LicensePanel::kFontHeight)

        m_license.onLicenseReceived = [this]
        {
            m_status.setText ("Drag the file icon to a removable drive and upload it to your license page.\nThen drop the license file from there back here.",
                              juce::sendNotification);

            resized();
        };

        m_license.onLicenseReceived();
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        m_offlineButton.setBounds (bounds.removeFromBottom (getHeight() / 2).withSizeKeepingCentre (getHeight() / 2, getHeight() / 2));
        m_status.setBounds (bounds.withSizeKeepingCentre (getWidth(), 50));
    }

    LicensePanel&          m_owner;
    foleys::License        m_license;
    juce::Label            m_status;
    foleys::FileDragButton m_offlineButton { "Offline Activation", juce::DrawableButton::ButtonStyle::ImageAboveTextLabel };
};

// ================================================================================

void LicensePanel::TabButtonLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&, bool shouldDrawButtonAsHighlighted, bool)
{
    const auto colour    = juce::Colours::green;  // findColour (juce::TextButton::buttonOnColourId);
    const auto y         = float (button.getHeight()) - 3.0f;
    const auto lineWidth = 2.0f;

    if (button.getToggleState())
    {
        g.setColour (shouldDrawButtonAsHighlighted ? colour.brighter() : colour);
        g.drawLine (lineWidth, y, button.getWidth() - lineWidth * 2.0f, y, 2.0f * lineWidth);
    }
    else if (shouldDrawButtonAsHighlighted)
    {
        g.setColour (colour.darker());
        g.drawLine (lineWidth, y, button.getWidth() - lineWidth * 2.0f, y, 2.0f * lineWidth);
    }
}

// ================================================================================

LicensePanel::LicensePanel (bool embed) : m_embedded (embed)
{
    if (!isColourSpecified (buttonColourId))
        setColour (buttonColourId, juce::Colour { 55, 69, 129 });
    if (!isColourSpecified (inactiveTextColourId))
        setColour (inactiveTextColourId, juce::Colour { 40, 41, 46 });
    if (!isColourSpecified (footerColourId))
        setColour (footerColourId, juce::Colours::silver);

    addAndMakeVisible (m_demoTabButton);
    addAndMakeVisible (m_activationTabButton);
    addAndMakeVisible (m_offlineTabButton);
    m_demoTabButton.setClickingTogglesState (true);
    m_activationTabButton.setClickingTogglesState (true);
    m_offlineTabButton.setClickingTogglesState (true);
    m_demoTabButton.setRadioGroupId (100);
    m_activationTabButton.setRadioGroupId (100);
    m_offlineTabButton.setRadioGroupId (100);
    m_demoTabButton.onClick       = [this] { setTab (Tab::Demo); };
    m_activationTabButton.onClick = [this] { setTab (Tab::Activation); };
    m_offlineTabButton.onClick    = [this] { setTab (Tab::Offline); };
    m_demoTabButton.setLookAndFeel (&m_tabButtonLookAndFeel);
    m_activationTabButton.setLookAndFeel (&m_tabButtonLookAndFeel);
    m_offlineTabButton.setLookAndFeel (&m_tabButtonLookAndFeel);

    addChildComponent (m_title);
    addChildComponent (m_copyright);
    addAndMakeVisible (m_closeButton);
    addAndMakeVisible (m_refreshButton);
    addAndMakeVisible (m_status);
    addAndMakeVisible (m_timestamp);

    m_title.setJustificationType (juce::Justification::centred);
    m_title.setText (LicenseData::productName, juce::dontSendNotification);

    SET_JUCE_FONT (m_title, kTitleFontHeight)
    SET_JUCE_FONT (m_timestamp, kSmallFontHeight)
    SET_JUCE_FONT (m_copyright, kSmallFontHeight)

    const auto inactiveTextColour = findColour (inactiveTextColourId);

    const auto footerColour = findColour (footerColourId);
    m_copyright.setJustificationType (juce::Justification::left);
    m_copyright.setColour (juce::Label::textColourId, footerColour);
    m_copyright.setText (LicenseData::copyright, juce::dontSendNotification);

    m_timestamp.setJustificationType (juce::Justification::right);
    m_timestamp.setColour (juce::Label::textColourId, footerColour);

    m_closeButton.onClick = [this]
    {
        if (m_license.isAllowed() && onCloseRequest)
            onCloseRequest();
    };

    m_refreshButton.onClick     = [this] { m_license.syncLicense(); };
    m_license.onLicenseReceived = [this] { update(); };

    m_demoTabButton.triggerClick();

    update();
}

LicensePanel::~LicensePanel()
{
    setLookAndFeel (nullptr);
}

void LicensePanel::setTab (Tab currentTab)
{

    switch (currentTab)
    {
        case Tab::Demo: m_currentTab = std::make_unique<DemoTab> (*this); break;
        case Tab::Activation: m_currentTab = std::make_unique<ActivationTab> (*this); break;
        case Tab::Offline: m_currentTab = std::make_unique<OfflineTab> (*this); break;
        default: m_currentTab = std::make_unique<ActivationTab> (*this); break;
    }

    addAndMakeVisible (m_currentTab.get());
    resized();
}

void LicensePanel::addLinkButton (std::unique_ptr<juce::Button>&& newButton)
{
    addAndMakeVisible (newButton.get());
    m_linkButtons.push_back (std::move (newButton));
}

void LicensePanel::update()
{
    m_closeButton.setVisible (m_license.isAllowed());

    if (auto checked = m_license.lastChecked())
        m_timestamp.setText (TRANS ("Last checked: ") + juce::String (Helpers::formatDateTime (*checked, "%d. %m. %Y %H:%M")) + " UTC", juce::sendNotification);
    else
        m_timestamp.setText (TRANS ("Never checked"), juce::sendNotification);

    if (onLicenseChanged)
        onLicenseChanged (m_license);
}

void LicensePanel::requestClose()
{
    if (onCloseRequest)
        onCloseRequest();
}

bool LicensePanel::isInterestedInFileDrag (const juce::StringArray& files)
{
    if (files.size() != 1)
        return false;

    auto file = juce::File (files.getReference (0));

    return file.hasFileExtension (".lic") && file.existsAsFile();
}

void LicensePanel::filesDropped (const juce::StringArray& files, [[maybe_unused]] int x, [[maybe_unused]] int y)
{
    auto file = juce::File (files.getReference (0));

    if (m_license.setOfflineLicenseData (file.loadFileAsString().toStdString()))
    {
        if (onLicenseChanged)
            onLicenseChanged (m_license);
    }
}

void LicensePanel::paint (juce::Graphics& g)
{
    // if (onPaint)
    //     onPaint (g);
    // else
    g.fillAll (juce::Colours::black.withAlpha (0.8f));
}

void LicensePanel::resized()
{
    auto bounds = getLocalBounds().reduced (30);
    if (!m_embedded)
    {
        m_title.setBounds (bounds.removeFromTop (kMaxRowHeight));
        m_copyright.setBounds (bounds.removeFromBottom (kMaxRowHeight).reduced (0, 5));
    }

    m_title.setVisible (!m_embedded);
    m_copyright.setVisible (!m_embedded);

    auto tabRow = bounds.removeFromTop (40);
    auto button = tabRow.withWidth (tabRow.getWidth() / 3).reduced (30, 0);
    m_demoTabButton.setBounds (button.withX (tabRow.getX() + 30));
    m_activationTabButton.setBounds (button.withX (tabRow.getX() + 90 + button.getWidth()));
    m_offlineTabButton.setBounds (button.withX (tabRow.getX() + 120 + 2 * button.getWidth()));

    if (m_currentTab)
        m_currentTab->setBounds (bounds.removeFromTop (bounds.getHeight() / 2));

    m_timestamp.setBounds (bounds.removeFromBottom (40).reduced (60, 10));
    m_refreshButton.setBounds (m_timestamp.getRight() + 5, m_timestamp.getY(), m_timestamp.getHeight(), m_timestamp.getHeight());

    if (!m_linkButtons.empty())
    {
        auto       linksArea = bounds.reduced (50, 10);
        const auto w         = bounds.getWidth() / int (m_linkButtons.size());
        for (const auto& button: m_linkButtons)
            button->setBounds (linksArea.removeFromLeft (w).reduced (10));
    }
}

}  // namespace foleys
