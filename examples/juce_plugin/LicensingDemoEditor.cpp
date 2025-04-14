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
    licensePanel->setButtonIcon (foleys::LicensePanel::Manual, BinaryData::pdficon_svg, BinaryData::pdficon_svgSize);
    licensePanel->setButtonIcon (foleys::LicensePanel::UserPage, BinaryData::keyicon_svg, BinaryData::keyicon_svgSize);
    licensePanel->setButtonIcon (foleys::LicensePanel::ProductPage, BinaryData::wwwicon_svg, BinaryData::wwwicon_svgSize);
    licensePanel->setButtonIcon (foleys::LicensePanel::Close, BinaryData::closeicon_svg, BinaryData::closeicon_svgSize);
    licensePanel->setButtonIcon (foleys::LicensePanel::Refresh, BinaryData::refreshicon_svg, BinaryData::refreshicon_svgSize);
    licensePanel->setButtonIcon (foleys::LicensePanel::OfflineAuth, BinaryData::saveicon_svg, BinaryData::saveicon_svgSize);

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
