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

LicensePanel::LicensePanel()
{
    if (!isColourSpecified (buttonColourId))
        setColour (buttonColourId, juce::Colour { 55, 69, 129 });
    if (!isColourSpecified (inactiveTextColourId))
        setColour (inactiveTextColourId, juce::Colour { 40, 41, 46 });
    if (!isColourSpecified (footerColourId))
        setColour (footerColourId, juce::Colours::silver);

    addChildComponent (m_title);
    addChildComponent (m_copyright);
    addChildComponent (m_codeEditor);
    addChildComponent (m_codeLabel);
    addChildComponent (m_submitCodeButton);
    addAndMakeVisible (m_closeButton);
    addAndMakeVisible (m_refreshButton);
    addAndMakeVisible (m_status);
    addAndMakeVisible (m_demo);
    addAndMakeVisible (m_manualButton);
    addAndMakeVisible (m_homeButton);
    addAndMakeVisible (m_websiteButton);
    addAndMakeVisible (m_timestamp);
    addAndMakeVisible (m_deactivateButton);
    addAndMakeVisible (m_offlineButton);


    m_title.setJustificationType (juce::Justification::centred);
    m_title.setText (LicenseData::productName, juce::dontSendNotification);

#if JUCE_VERSION >= 0x080000
    m_title.setFont (juce::FontOptions { kTitleFontHeight });
    m_codeLabel.setFont (juce::FontOptions { kFontHeight });
    m_timestamp.setFont (juce::FontOptions { kSmallFontHeight });
    m_copyright.setFont (juce::FontOptions { kSmallFontHeight });
#else
    m_title.setFont (juce::Font (kTitleFontHeight));
    m_codeLabel.setFont (juce::Font (kFontHeight));
    m_timestamp.setFont (juce::Font (kSmallFontHeight));
    m_copyright.setFont (juce::Font (kSmallFontHeight));
#endif

    const auto inactiveTextColour = findColour (inactiveTextColourId);
    m_codeEditor.setColour (juce::TextEditor::backgroundColourId, inactiveTextColour);
    m_codeEditor.setTextToShowWhenEmpty ("XXXX-YYYY-ZZZZ-WWWW", juce::Colours::grey);
    m_codeEditor.setJustification (juce::Justification::centred);

    m_deactivateButton.setColour (juce::TextButton::buttonColourId, findColour (buttonColourId));

    m_codeLabel.setJustificationType (juce::Justification::centred);

    const auto footerColour = findColour (footerColourId);
    m_copyright.setJustificationType (juce::Justification::left);
    m_copyright.setColour (juce::Label::textColourId, footerColour);
    m_copyright.setText (LicenseData::copyright, juce::dontSendNotification);

    m_timestamp.setJustificationType (juce::Justification::right);
    m_timestamp.setColour (juce::Label::textColourId, footerColour);

    m_manualButton.onClick  = [] { juce::URL (LicenseData::manualUrl).launchInDefaultBrowser(); };
    m_homeButton.onClick    = [] { juce::URL (LicenseData::authServerUrl).launchInDefaultBrowser(); };
    m_websiteButton.onClick = [] { juce::URL (LicenseData::buyUrl).launchInDefaultBrowser(); };

    m_closeButton.onClick = [this]
    {
        if (license.isAllowed() && onCloseRequest)
            onCloseRequest();
    };

    m_refreshButton.onClick = [this] { license.syncLicense(); };

    license.onLicenseReceived = [this] { update(); };

    m_demo.onClick = [this]
    {
        if (license.isActivated() || license.isDemo())
        {
            if (onCloseRequest)
                onCloseRequest();
        }
        else if (license.canDemo())
        {
            license.startDemo();
        }
    };

    m_deactivateButton.onClick = [this]
    {
        if (license.isActivated())
            license.deactivate();
    };

    m_submitCodeButton.onClick = [this]
    {
        if (!m_codeEditor.isEmpty())
        {
            activate (m_codeEditor.getText(), 0);
        }
    };

    update();
}

void LicensePanel::setStyle (int styleFlags)
{
    m_style = styleFlags;

    m_copyright.setVisible (m_style & ShowCopyright);
    m_title.setVisible (m_style & ShowTitle);
}

void LicensePanel::update()
{
    setStyle (m_style);

    switch (license.getState())
    {
        case foleys::License::Unknown: DBG ("License state: Unknown."); break;
        case foleys::License::Error: DBG ("License state: Error."); break;
        case foleys::License::DemoExpired: DBG ("License state: Demo Expired."); break;
        case foleys::License::Expired: DBG ("License state: Expired."); break;
        case foleys::License::DemoAvailable: DBG ("License state: Demo Available."); break;
        case foleys::License::DemoRunning: DBG ("License state: Demo Running."); break;
        case foleys::License::ActivationsUsed: DBG ("License state: No Activations Left."); break;
        case foleys::License::ActivationsAvailable: DBG ("License state: Activations Available."); break;
        case foleys::License::Activated: DBG ("License state: Activated."); break;
        default: break;
    }

    m_closeButton.setVisible (license.isAllowed());
    m_demo.setEnabled (license.canDemo() || license.isAllowed());

    const auto isActivated = license.isActivated();
    m_deactivateButton.setVisible (isActivated);
    m_codeEditor.setVisible (!isActivated);
    m_submitCodeButton.setVisible (!isActivated);
    m_codeLabel.setVisible (!isActivated);

    if (auto checked = license.lastChecked())
        m_timestamp.setText (TRANS ("Checked: ") + juce::String (Helpers::formatDateTime (*checked, "%d. %m. %Y %H:%M")) + " UTC", juce::sendNotification);
    else
        m_timestamp.setText (TRANS ("Never checked"), juce::sendNotification);

    if (isActivated)
    {
        if (license.expires())
        {
            const auto date = *license.expires();
            m_demo.setButtonText (LicenseData::productName + TRANS (" expires ") + juce::String (Helpers::formatDateTime (date, "%d. %m %Y")));
        }
        else
            m_demo.setButtonText (LicenseData::productName + TRANS (" activated"));
    }
    else if (license.isDemo())
    {
        m_demo.setButtonText (TRANS ("Days to evaluate: ") + juce::String (license.demoDaysLeft()));
    }
    else if (!license.canDemo())
    {
        m_demo.setButtonText (TRANS ("Demo expired, please buy a license"));
    }

    juce::String lastError (license.getLastErrorString());
    if (lastError.isNotEmpty())
        m_status.setText (lastError, juce::dontSendNotification);
    else if (license.isExpired())
    {
        m_status.setText ("Your license expired on " + juce::String (Helpers::formatDateTime (*license.expires(), "%d. %b %Y")), juce::dontSendNotification);
    }
    else if (isActivated)
        m_status.setText ("", juce::sendNotification);
    else if (license.isDemo())
        m_status.setText ({}, juce::dontSendNotification);
    else if (license.canDemo())
        m_status.setText ("Hit the Demo button to start your free " + juce::String (license.demoDaysLeft()) + " days trial", juce::dontSendNotification);
    else
        m_status.setText ("If you bought a license enter your email and hit Activate", juce::dontSendNotification);

    // Show a panel to deactivate a machine
    const auto activations = license.getActivations();
    if (!m_codeEditor.isEmpty() && !activations.empty())
    {
        auto panel          = std::make_unique<LicenseDeactivate>();
        panel->onDeactivate = [this] (size_t idToDeactivate) { activate (m_codeEditor.getText(), idToDeactivate); };
        panel->setCloseFunction ([this] { m_deactivationPanel.reset(); });

        addAndMakeVisible (panel.get());
        m_deactivationPanel = std::move (panel);
        resized();
    }
    else
    {
        m_deactivationPanel.reset();
    }

    if (onLicenseChanged)
        onLicenseChanged (license);
}

void LicensePanel::activate (const juce::String& serial, size_t deactivateID)
{
    std::vector<std::pair<std::string, std::string>> data = { { LicenseID::computer, juce::SystemStats::getComputerName().toRawUTF8() },
                                                              { LicenseID::user, juce::SystemStats::getFullUserName().toRawUTF8() },
                                                              { LicenseID::serial, serial.toRawUTF8() } };

    if (deactivateID > 0)
        data.emplace_back (LicenseID::deactivate, std::to_string (deactivateID));

    license.activate (data);
}

void LicensePanel::setButtonIcon (Button buttonType, juce::Colour buttonColour, const char* imageData, size_t imageDataSize)
{
    auto image = juce::DrawableComposite::createFromImageData (imageData, imageDataSize);
    image->replaceColour (juce::Colours::black, buttonColour);
    const auto setupButton = [&] (juce::DrawableButton& button)
    {
        button.setImages (image.get());
        button.setColour (juce::DrawableButton::backgroundColourId, juce::Colours::darkgrey);
    };

    switch (buttonType)
    {
        case Close: setupButton (m_closeButton); break;
        case Refresh: setupButton (m_refreshButton); break;
        case Manual: setupButton (m_manualButton); break;
        case UserPage: setupButton (m_homeButton); break;
        case ProductPage: setupButton (m_websiteButton); break;
        case OfflineAuth: setupButton (m_offlineButton); break;
        case Unknown: [[fallthrough]];
        default: break;
    }
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

    if (license.setOfflineLicenseData (file.loadFileAsString().toStdString()))
    {
        if (onLicenseChanged)
            onLicenseChanged (license);
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
    if (onResized)
    {
        onResized (getLocalBounds());
        return;
    }

    auto area = getLocalBounds().withSizeKeepingCentre (std::min (getWidth(), kMaxWidth), getHeight());

    const bool hasTitle  = (m_style & ShowTitle);
    const auto numRows   = 8 + (hasTitle ? 1 : 0);
    const auto rowHeight = std::max (kMaxRowHeight, (area.getHeight() / numRows) - kGrid);

    if (m_deactivationPanel)
    {
        m_deactivationPanel->setBounds (area);
        m_deactivationPanel->toFront (false);
    }

    area.removeFromTop (kGrid);

    auto topRow = area.removeFromTop (rowHeight).withSizeKeepingCentre (area.getWidth(), kMaxRowHeight);
    m_closeButton.setBounds (topRow.removeFromRight (kMaxRowHeight));
    topRow.removeFromLeft (rowHeight);
    m_title.setBounds (topRow);

    area.removeFromTop (kGrid);

    if (hasTitle)
        area.removeFromTop (rowHeight + kGrid);

    m_codeLabel.setBounds (area.removeFromTop (rowHeight).withSizeKeepingCentre (area.getWidth(), kMaxRowHeight));

    area.removeFromTop (kGrid);

    auto       codeRow         = area.removeFromTop (rowHeight).withSizeKeepingCentre (area.getWidth() - (12 * kGrid), kMaxRowHeight);
    const auto codeButtonWidth = codeRow.getWidth() / 4;
    m_submitCodeButton.setBounds (codeRow.removeFromRight (codeButtonWidth).reduced (2 * kGrid, 0));
    m_codeEditor.setBounds (codeRow.reduced (2 * kGrid, 0));

    area.removeFromTop (kGrid);

    auto demoRow = area.removeFromTop (rowHeight).withSizeKeepingCentre (area.getWidth() - (12 * kGrid), kMaxRowHeight);
    m_refreshButton.setBounds (demoRow.removeFromRight (codeButtonWidth).reduced (2 * kGrid, 0));
    m_demo.setBounds (demoRow.reduced (2 * kGrid, 0));

    area.removeFromTop (kGrid);

    auto statusRow = area.removeFromTop (rowHeight).withSizeKeepingCentre (area.getWidth() - (12 * kGrid), kMaxRowHeight);
    m_deactivateButton.setBounds (statusRow.removeFromRight (codeButtonWidth).reduced (2 * kGrid, 0));
    m_status.setBounds (statusRow.reduced (2 * kGrid, 0));

    area.removeFromTop (kGrid);

    const auto footerBounds = area.removeFromBottom (rowHeight).withSizeKeepingCentre (getWidth(), kMaxRowHeight);
    m_timestamp.setBounds (footerBounds.withTrimmedTop (2 * kGrid));
    m_copyright.setBounds (footerBounds.withTrimmedTop (2 * kGrid));

    area.removeFromBottom (kGrid);

    auto       buttonRow = area.removeFromBottom ((2 * rowHeight) + kGrid).withSizeKeepingCentre (area.getWidth(), (2 * kMaxRowHeight) + kGrid);
    const auto w         = buttonRow.getWidth() / 4;
    m_homeButton.setBounds (buttonRow.removeFromLeft (w).withTrimmedRight (10));
    m_websiteButton.setBounds (buttonRow.removeFromLeft (w).withTrimmedRight (10));
    m_offlineButton.setBounds (buttonRow.removeFromLeft (w).withTrimmedRight (10));
    m_manualButton.setBounds (buttonRow.reduced (10, 0));
}

}  // namespace foleys
