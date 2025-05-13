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

    addChildComponent (title);
    addChildComponent (copyright);
    addAndMakeVisible (closeButton);
    addAndMakeVisible (refreshButton);
    addAndMakeVisible (enterSerial);
    addAndMakeVisible (code);
    addAndMakeVisible (submit);
    addAndMakeVisible (status);
    addAndMakeVisible (demo);
    addAndMakeVisible (manualButton);
    addAndMakeVisible (homeButton);
    addAndMakeVisible (websiteButton);
    addAndMakeVisible (timestamp);
    addAndMakeVisible (deactivate);
    addAndMakeVisible (offlineButton);


    title.setJustificationType (juce::Justification::centred);
    title.setText (LicenseData::productName, juce::dontSendNotification);

#if JUCE_VERSION >= 0x080000
    title.setFont (juce::FontOptions { kTitleFontHeight });
    enterSerial.setFont (juce::FontOptions { kFontHeight });
    timestamp.setFont (juce::FontOptions { kSmallFontHeight });
    copyright.setFont (juce::FontOptions { kSmallFontHeight });
#else
    title.setFont (juce::Font (kTitleFontHeight));
    enterSerial.setFont (juce::Font (kFontHeight));
    timestamp.setFont (juce::Font (kSmallFontHeight));
    copyright.setFont (juce::Font (kSmallFontHeight));
#endif

    const auto inactiveTextColour = findColour (inactiveTextColourId);
    code.setColour (juce::TextEditor::backgroundColourId, inactiveTextColour);
    code.setTextToShowWhenEmpty ("XXXX-YYYY-ZZZZ-WWWW", juce::Colours::grey);
    code.setJustification (juce::Justification::centred);

    deactivate.setColour (juce::TextButton::buttonColourId, findColour (buttonColourId));

    enterSerial.setJustificationType (juce::Justification::centred);

    status.setColour (juce::Label::textColourId, inactiveTextColour);

    const auto footerColour = findColour (footerColourId);
    copyright.setJustificationType (juce::Justification::left);
    copyright.setColour (juce::Label::textColourId, footerColour);
    copyright.setText (LicenseData::copyright, juce::dontSendNotification);

    timestamp.setJustificationType (juce::Justification::right);
    timestamp.setColour (juce::Label::textColourId, footerColour);

    manualButton.onClick  = [] { juce::URL (LicenseData::manualUrl).launchInDefaultBrowser(); };
    homeButton.onClick    = [] { juce::URL (LicenseData::authServerUrl).launchInDefaultBrowser(); };
    websiteButton.onClick = [] { juce::URL (LicenseData::buyUrl).launchInDefaultBrowser(); };

    closeButton.onClick = [this]
    {
        if (license.isAllowed() && onCloseRequest)
            onCloseRequest();
    };

    refreshButton.onClick = [this] { license.syncLicense(); };

    license.onLicenseReceived = [this] { update(); };

    demo.onClick = [this]
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

    deactivate.onClick = [this]
    {
        if (license.isActivated())
            license.deactivate();
    };

    submit.onClick = [this]
    {
        if (!code.isEmpty())
        {
            activate (code.getText(), 0);
        }
    };

    update();
}

void LicensePanel::setStyle (int styleFlags)
{
    m_style = styleFlags;

    copyright.setVisible (m_style & ShowCopyright);
    title.setVisible (m_style & ShowTitle);
}

void LicensePanel::update()
{
    setStyle (m_style);

    closeButton.setVisible (license.isAllowed());
    demo.setEnabled (license.canDemo() || license.isAllowed());
    deactivate.setVisible (license.isActivated());

    if (auto checked = license.lastChecked())
    {
        timestamp.setText (TRANS ("Checked: ") + juce::String (Helpers::formatDateTime (*checked, "%d. %m. %Y %H:%M")) + " UTC", juce::sendNotification);
    }
    else
    {
        timestamp.setText (TRANS ("Never checked"), juce::sendNotification);
    }

    if (license.isActivated())
    {
        if (license.expires())
        {
            const auto date = *license.expires();
            demo.setButtonText (LicenseData::productName + TRANS (" expires ") + juce::String (Helpers::formatDateTime (date, "%d. %m %Y")));
        }
        else
            demo.setButtonText (LicenseData::productName + TRANS (" activated"));
    }
    else if (license.isDemo())
        demo.setButtonText (TRANS ("Days to evaluate: ") + juce::String (license.demoDaysLeft()));
    else if (!license.canDemo())
        demo.setButtonText (TRANS ("Demo expired, please buy a license"));

    juce::String lastError (license.getLastErrorString());
    if (lastError.isNotEmpty())
        status.setText (lastError, juce::dontSendNotification);
    else if (license.isExpired())
    {
        status.setText ("Your license expired on " + juce::String (Helpers::formatDateTime (*license.expires(), "%d. %b %Y")), juce::dontSendNotification);
    }
    else if (license.isActivated())
        status.setText ("", juce::sendNotification);
    else if (license.isDemo())
        status.setText ({}, juce::dontSendNotification);
    else if (license.canDemo())
        status.setText ("Hit the Demo button to start your free " + juce::String (license.demoDaysLeft()) + " days trial", juce::dontSendNotification);
    else
        status.setText ("If you bought a license enter your email and hit Activate", juce::dontSendNotification);

    // Show a panel to deactivate a machine
    if (!code.isEmpty() && !license.getActivations().empty())
    {
        auto panel          = std::make_unique<LicenseDeactivate>();
        panel->onDeactivate = [this] (size_t idToDeactivate) { activate (code.getText(), idToDeactivate); };
        panel->setCloseFunction ([this] { deactivationPanel.reset(); });

        addAndMakeVisible (panel.get());
        deactivationPanel = std::move (panel);
        resized();
    }
    else
    {
        deactivationPanel.reset();
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
        case Close: setupButton (closeButton); break;
        case Refresh: setupButton (refreshButton); break;
        case Manual: setupButton (manualButton); break;
        case UserPage: setupButton (homeButton); break;
        case ProductPage: setupButton (websiteButton); break;
        case OfflineAuth: setupButton (offlineButton); break;
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

    if (deactivationPanel)
    {
        deactivationPanel->setBounds (area);
        deactivationPanel->toFront (false);
    }

    area.removeFromTop (kGrid);

    auto topRow = area.removeFromTop (rowHeight).withSizeKeepingCentre (area.getWidth(), kMaxRowHeight);
    closeButton.setBounds (topRow.removeFromRight (kMaxRowHeight));
    topRow.removeFromLeft (rowHeight);
    title.setBounds (topRow);

    area.removeFromTop (kGrid);

    if (hasTitle)
        area.removeFromTop (rowHeight + kGrid);

    enterSerial.setBounds (area.removeFromTop (rowHeight).withSizeKeepingCentre (area.getWidth(), kMaxRowHeight));

    area.removeFromTop (kGrid);

    auto       codeRow         = area.removeFromTop (rowHeight).withSizeKeepingCentre (area.getWidth() - (12 * kGrid), kMaxRowHeight);
    const auto codeButtonWidth = codeRow.getWidth() / 4;
    submit.setBounds (codeRow.removeFromRight (codeButtonWidth).reduced (2 * kGrid, 0));
    code.setBounds (codeRow.reduced (2 * kGrid, 0));

    area.removeFromTop (kGrid);

    auto demoRow = area.removeFromTop (rowHeight).withSizeKeepingCentre (area.getWidth() - (12 * kGrid), kMaxRowHeight);
    refreshButton.setBounds (demoRow.removeFromRight (codeButtonWidth).reduced (2 * kGrid, 0));
    demo.setBounds (demoRow.reduced (2 * kGrid, 0));

    area.removeFromTop (kGrid);

    auto statusRow = area.removeFromTop (rowHeight).withSizeKeepingCentre (area.getWidth() - (12 * kGrid), kMaxRowHeight);
    deactivate.setBounds (statusRow.removeFromRight (codeButtonWidth).reduced (2 * kGrid, 0));
    status.setBounds (statusRow.reduced (2 * kGrid, 0));

    area.removeFromTop (kGrid);

    const auto footerBounds = area.removeFromBottom (rowHeight).withSizeKeepingCentre (getWidth(), kMaxRowHeight);
    timestamp.setBounds (footerBounds.withTrimmedTop (2 * kGrid));
    copyright.setBounds (footerBounds.withTrimmedTop (2 * kGrid));

    area.removeFromBottom (kGrid);

    auto       buttonRow = area.removeFromBottom ((2 * rowHeight) + kGrid).withSizeKeepingCentre (area.getWidth(), (2 * kMaxRowHeight) + kGrid);
    const auto w         = buttonRow.getWidth() / 4;
    homeButton.setBounds (buttonRow.removeFromLeft (w).withTrimmedRight (10));
    websiteButton.setBounds (buttonRow.removeFromLeft (w).withTrimmedRight (10));
    offlineButton.setBounds (buttonRow.removeFromLeft (w).withTrimmedRight (10));
    manualButton.setBounds (buttonRow.reduced (10, 0));
}

}  // namespace foleys
