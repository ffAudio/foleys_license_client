//
// Created by Daniel Walz on 11.10.24.
//

#include "foleys_LicenseDeactivate.h"

namespace foleys
{

LicenseDeactivate::LicenseDeactivate()
{
    activations = license.getActivations();
    deactivations.setRowHeight (48);
    deactivations.setModel (this);
    deactivations.setColour (juce::ListBox::backgroundColourId, juce::Colours::black);
    addAndMakeVisible (deactivations);
    addAndMakeVisible (closeButton);
}

LicenseDeactivate::~LicenseDeactivate()
{
    deactivations.setModel (nullptr);
}

void LicenseDeactivate::resized()
{
    auto area = getLocalBounds().reduced (20);
    area.removeFromTop (50);
    closeButton.setBounds (area.removeFromBottom (24));
    deactivations.setBounds (area.withTrimmedBottom (5));
}

void LicenseDeactivate::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black.withAlpha (0.8f));
    g.setColour (juce::Colours::silver);
    g.setFont (24.0f);
    g.drawFittedText ("Select a machine to deactivate instead", getLocalBounds().reduced (20).withHeight (40), juce::Justification::centred, 2);
}

int LicenseDeactivate::getNumRows()
{
    return static_cast<int> (activations.size());
}

class DeactivateItem : public juce::Component
{
public:
    explicit DeactivateItem (LicenseDeactivate& ownerToUse) : owner (ownerToUse) { addAndMakeVisible (deactivate); }

    void setActivation (const foleys::Activation& activationToUse)
    {
        activation         = activationToUse;
        deactivate.onClick = [this]
        {
            if (owner.onDeactivate)
                owner.onDeactivate (activation.index);
        };
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);
        g.setColour (juce::Colours::silver);
        g.setFont (20.0f);
        g.drawFittedText (activation.computer, 10, 4, getWidth() - 110, 24, juce::Justification::left, 1);
        g.setFont (14.0f);
        g.drawFittedText (activation.user, 10, 28, getWidth() - 110, 18, juce::Justification::left, 1);
    }

    void resized() override { deactivate.setBounds (getRight() - 90, 6, 80, 24); }

private:
    LicenseDeactivate& owner;
    foleys::Activation activation;
    juce::TextButton   deactivate { "deactivate", "Deactivate" };
};

juce::Component* LicenseDeactivate::refreshComponentForRow (int rowNumber, [[maybe_unused]] bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    if (auto* comp = dynamic_cast<DeactivateItem*> (existingComponentToUpdate))
    {
        comp->setActivation (activations[size_t (rowNumber)]);
        return comp;
    }

    if (existingComponentToUpdate)
        delete (existingComponentToUpdate);

    auto* comp = new DeactivateItem (*this);
    comp->setActivation (activations[size_t (rowNumber)]);
    return comp;
}

}  // namespace foleys