//
// Created by Daniel Walz on 12.08.24.
//

#include "LicensingDemoEditor.h"
#include "LicensingDemoProcessor.h"


LicensingDemoEditor::LicensingDemoEditor (LicensingDemoProcessor& p) : juce::AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible (aboutButton);
    aboutButton.onClick = [this] { popupHolder.showPopup (std::make_unique<foleys::LicensePanel>()); };

    setResizable (true, true);
    setSize (640, 480);

    foleys::License license;
    if (license.shouldShowPopup())
    {
        popupHolder.showPopup (std::make_unique<foleys::LicensePanel>());
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
