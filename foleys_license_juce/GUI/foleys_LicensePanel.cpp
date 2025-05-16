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

struct DemoTab : juce::Component
{
    explicit DemoTab (LicensePanel& owner) : m_owner (owner)
    {
        addAndMakeVisible (m_demoStatus);
        m_demoStatus.setJustificationType (juce::Justification::centred);

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
            if (m_license.isDemo())
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
                    m_status.setText ("Your license expired on ", juce::sendNotification);
                    addButton (TRANS ("Renew license"), [] { juce::URL (LicenseData::buyUrl).launchInDefaultBrowser(); });
                }
                else if (m_license.expires())
                {
                    m_status.setText ("Your license will expire on ", juce::sendNotification);
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

        m_license.onLicenseReceived = [this]
        {
            m_status.setText ("Drag the file icon to a removable drive and upload it to your license page. Then drop the license file from there back here.",
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

LicensePanel::LicensePanel (bool embed) : m_embedded (embed)
{
    if (!isColourSpecified (buttonColourId))
        setColour (buttonColourId, juce::Colour { 55, 69, 129 });
    if (!isColourSpecified (inactiveTextColourId))
        setColour (inactiveTextColourId, juce::Colour { 40, 41, 46 });
    if (!isColourSpecified (footerColourId))
        setColour (footerColourId, juce::Colours::silver);

    addAndMakeVisible (m_actionTabs);

    addChildComponent (m_title);
    addChildComponent (m_copyright);
    addAndMakeVisible (m_closeButton);
    addAndMakeVisible (m_refreshButton);
    addAndMakeVisible (m_status);
    addAndMakeVisible (m_timestamp);

    m_title.setJustificationType (juce::Justification::centred);
    m_title.setText (LicenseData::productName, juce::dontSendNotification);

#if JUCE_VERSION >= 0x080000
    m_title.setFont (juce::FontOptions { kTitleFontHeight });
    m_codeLabel.setFont (juce::FontOptions { kFontHeight });
    m_timestamp.setFont (juce::FontOptions { kSmallFontHeight });
    m_copyright.setFont (juce::FontOptions { kSmallFontHeight });
#else
    m_title.setFont (juce::Font (kTitleFontHeight));
    m_timestamp.setFont (juce::Font (kSmallFontHeight));
    m_copyright.setFont (juce::Font (kSmallFontHeight));
#endif

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

    m_refreshButton.onClick = [this] { m_license.syncLicense(); };

    m_license.onLicenseReceived = [this] { update(); };

    update();
}

void LicensePanel::initialize()
{
    m_actionTabs.clearTabs();

    m_actionTabs.addTab (TRANS ("Demo"), juce::Colours::transparentBlack, new DemoTab (*this), true);
    m_actionTabs.addTab (TRANS ("Activate"), juce::Colours::transparentBlack, new ActivationTab (*this), true);

    auto* offlineTab = new OfflineTab (*this);
    offlineTab->m_offlineButton.setImages (m_offlineIcon.get());

    m_actionTabs.addTab (TRANS ("Offline"), juce::Colours::transparentBlack, offlineTab, true);
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
    auto bounds = getLocalBounds().reduced (kGrid);
    if (!m_embedded)
    {
        m_title.setBounds (bounds.removeFromTop (kMaxRowHeight));
        m_copyright.setBounds (bounds.removeFromBottom (kMaxRowHeight).reduced (0, 5));
    }

    m_title.setVisible (!m_embedded);
    m_copyright.setVisible (!m_embedded);

    m_actionTabs.setBounds (bounds.removeFromTop (bounds.getHeight() / 2));

    m_timestamp.setBounds (bounds.removeFromBottom (50).reduced (60, 10));
    m_refreshButton.setBounds (m_timestamp.getRight() + 5, m_timestamp.getY(), m_timestamp.getHeight(), m_timestamp.getHeight());

    if (!m_linkButtons.empty())
    {
        auto       linksArea = bounds.reduced (50);
        const auto w         = bounds.getWidth() / int (m_linkButtons.size());
        for (const auto& button: m_linkButtons)
            button->setBounds (linksArea.removeFromLeft (w).reduced (10));
    }
}

}  // namespace foleys
