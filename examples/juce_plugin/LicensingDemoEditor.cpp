//
// Created by Daniel Walz on 12.08.24.
//

#include "LicensingDemoEditor.h"
#include "LicensingDemoProcessor.h"

#include "BinaryData.h"


namespace
{

void showLicensePanel (foleys::PopupHolder& popupHolder)
{
    auto licensePanel = std::make_unique<foleys::LicensePanel>();
    auto colour       = juce::Colours::silver;

    licensePanel->addLinkButton (foleys::LicensePanel::createButton ("Product Page", colour, BinaryData::wwwicon_svg, BinaryData::wwwicon_svgSize,
                                                                     [] { juce::URL (LicenseData::buyUrl).launchInDefaultBrowser(); }));
    licensePanel->addLinkButton (foleys::LicensePanel::createButton ("Manage Licenses", colour, BinaryData::keyicon_svg, BinaryData::keyicon_svgSize,
                                                                     [] { juce::URL (LicenseData::authServerUrl).launchInDefaultBrowser(); }));
    licensePanel->addLinkButton (foleys::LicensePanel::createButton ("Open Manual", colour, BinaryData::pdficon_svg, BinaryData::pdficon_svgSize,
                                                                     [] { juce::URL (LicenseData::manualUrl).launchInDefaultBrowser(); }));

    auto refresh = juce::DrawableComposite::createFromImageData (BinaryData::refreshicon_svg, BinaryData::refreshicon_svgSize);
    refresh->replaceColour (juce::Colours::black, juce::Colours::silver);  // button colour
    licensePanel->m_refreshButton.setImages (refresh.get());

    licensePanel->m_offlineIcon = juce::DrawableComposite::createFromImageData (BinaryData::saveicon_svg, BinaryData::saveicon_svgSize);
    licensePanel->m_offlineIcon->replaceColour (juce::Colours::black, juce::Colours::silver);  // button colour

    popupHolder.showPopup (std::move (licensePanel));
}

}  // namespace

LicensingDemoEditor::LicensingDemoEditor (LicensingDemoProcessor& p) : juce::AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible (aboutButton);
    aboutButton.onClick = [this] { showLicensePanel (popupHolder); };

    setResizable (true, true);
    setSize (640, 480);

    foleys::License license;
    if (license.shouldShowPopup())
    {
        showLicensePanel (popupHolder);
        license.setPopupWasShown (true);
    }
}

void LicensingDemoEditor::paint (juce::Graphics& g)
{
    g.fillAll (findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::silver);
    g.drawFittedText ("Hello user!", getLocalBounds(), juce::Justification::centred, 1);
}

void LicensingDemoEditor::resized()
{
    aboutButton.setBounds (getWidth() - 80, 0, 80, 20);
}
