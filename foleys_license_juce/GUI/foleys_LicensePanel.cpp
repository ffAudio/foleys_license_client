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
#include "BinaryData.h"

namespace foleys
{

LicensePanel::LicensePanel()
{
    const juce::Colour buttonColour { 55, 69, 129 };
    const juce::Colour labelColour { 40, 41, 46 };

    addAndMakeVisible (closeButton);
    addAndMakeVisible (refreshButton);
    addAndMakeVisible (title);
    addAndMakeVisible (enterSerial);
    addAndMakeVisible (code);
    addAndMakeVisible (submit);
    addAndMakeVisible (status);
    addAndMakeVisible (demo);
    addAndMakeVisible (manualButton);
    addAndMakeVisible (homeButton);
    addAndMakeVisible (websiteButton);
    addAndMakeVisible (copyright);
    addAndMakeVisible (timestamp);
    addAndMakeVisible (deactivate);
    addAndMakeVisible (offlineButton);

    title.setJustificationType (juce::Justification::centred);
    title.setColour (juce::Label::textColourId, juce::Colours::silver);
    title.setText (LicenseData::productName, juce::dontSendNotification);
    title.setFont (juce::Font (24.0f));

    code.setColour (juce::TextEditor::backgroundColourId, labelColour);
    code.setTextToShowWhenEmpty ("XXXX-YYYY-ZZZZ-WWWW", juce::Colours::grey);
    code.setJustification (juce::Justification::centred);

    submit.setColour (juce::TextButton::buttonColourId, buttonColour);
    submit.setColour (juce::TextButton::textColourOffId, juce::Colours::silver);
    demo.setColour (juce::TextButton::buttonColourId, buttonColour);
    demo.setColour (juce::TextButton::textColourOffId, juce::Colours::silver);
    deactivate.setColour (juce::TextButton::buttonColourId, buttonColour);
    deactivate.setColour (juce::TextButton::textColourOffId, juce::Colours::silver);

    enterSerial.setJustificationType (juce::Justification::centred);
    enterSerial.setColour (juce::Label::textColourId, juce::Colours::silver);
    enterSerial.setFont (juce::Font (18.0f));

    status.setJustificationType (juce::Justification::centred);
    status.setColour (juce::Label::textColourId, juce::Colours::silver);

    copyright.setJustificationType (juce::Justification::centred);
    copyright.setColour (juce::Label::textColourId, juce::Colours::silver);
    copyright.setText (LicenseData::copyright, juce::dontSendNotification);
    copyright.setFont (juce::Font (12.0f));

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

void LicensePanel::update()
{
    closeButton.setVisible (license.isAllowed());
    demo.setEnabled (license.canDemo() || license.isAllowed());
    deactivate.setVisible (license.isActivated());

    if (auto checked = license.lastChecked())
    {
        auto date = *checked;
        char buff[20];
        strftime (buff, 20, "%d. %m. %Y %H:%M", localtime (&date));
        timestamp.setText (TRANS ("Checked: ") + juce::String (buff) + " UTC", juce::sendNotification);
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
            char       buff[20];
            strftime (buff, 20, "%d. %m %Y", localtime (&date));
            demo.setButtonText (TRANS ("Plugin expires ") + juce::String (buff));
        }
        else
            demo.setButtonText (TRANS ("Plugin activated"));
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
        char dateString[64];
        auto expiryDate = *license.expires();
        std::strftime (dateString, 64, "%d. %b %Y", std::localtime (&expiryDate));
        status.setText ("Your license expired on " + juce::String (dateString), juce::dontSendNotification);
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

void LicensePanel::setButtonIcon (Button buttonType, const char* imageData, size_t imageDataSize)
{
    auto image = juce::DrawableComposite::createFromImageData (imageData, imageDataSize);
    image->replaceColour (juce::Colours::black, juce::Colours::silver);
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
    if (onPaint)
        onPaint (g);
    else
        g.fillAll (juce::Colours::black.withAlpha (0.8f));
}

void LicensePanel::resized()
{
    if (onResized)
    {
        onResized (getLocalBounds());
        return;
    }

    timestamp.setBounds (getWidth() - 222, getHeight() - 27, 200, 25);

    const auto buttonHeight = 30;

    auto area = getLocalBounds().reduced (40);
    area      = area.withSizeKeepingCentre (std::min (area.getWidth(), 400), area.getHeight());

    if (deactivationPanel)
    {
        deactivationPanel->setBounds (area);
        deactivationPanel->toFront (false);
    }

    closeButton.setBounds (getRight() - buttonHeight, 0, buttonHeight, buttonHeight);

    title.setBounds (area.removeFromTop (40).reduced (80, 0));
    copyright.setBounds (area.removeFromBottom (30).withTrimmedTop (10));

    auto       buttonArea = area.removeFromBottom (area.getHeight() / 4);
    const auto w          = buttonArea.getWidth() / 4;
    homeButton.setBounds (buttonArea.removeFromLeft (w).withTrimmedRight (10));
    websiteButton.setBounds (buttonArea.removeFromLeft (w).withTrimmedRight (10));
    offlineButton.setBounds (buttonArea.removeFromLeft (w).withTrimmedRight (10));
    manualButton.setBounds (buttonArea.reduced (10, 0));

    auto demoArea = area.removeFromBottom (60).withSizeKeepingCentre (std::min (area.getWidth(), 250), buttonHeight);
    refreshButton.setBounds (demoArea.removeFromRight (demoArea.getHeight()));
    demo.setBounds (demoArea.withTrimmedRight (10));

    auto third = area.getHeight() / 3;
    enterSerial.setBounds (area.removeFromTop (third));
    status.setBounds (area.removeFromBottom (third));

    area = area.withSizeKeepingCentre (area.getWidth() - 60, buttonHeight + 20);
    submit.setBounds (area.removeFromRight (area.getWidth() / 4).reduced (10));
    code.setBounds (area.reduced (10));
}

}  // namespace foleys
