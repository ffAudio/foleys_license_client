//
// Created by Daniel Walz on 12.08.24.
//

#ifndef FOLEYS_LICENSING_CLIENT_LICENSINGDEMOPROCESSOR_H
#define FOLEYS_LICENSING_CLIENT_LICENSINGDEMOPROCESSOR_H


#include <juce_audio_processors/juce_audio_processors.h>
#include <foleys_license_juce/foleys_license_juce.h>


class LicensingDemoProcessor : public juce::AudioProcessor
{
public:
    LicensingDemoProcessor();

    const juce::String getName() const override { return "Licensing Demo Plugin"; }

    void prepareToPlay (double sampleRate, int expectedNumSamples) override;
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;
    void releaseResources() override;

    // ================================================================================

    bool                        hasEditor() const override { return true; }
    juce::AudioProcessorEditor* createEditor() override;

    // ================================================================================

    void getStateInformation ([[maybe_unused]] juce::MemoryBlock& destData) override { }
    void setStateInformation ([[maybe_unused]] const void* data, [[maybe_unused]] int sizeInBytes) override { }

    // ================================================================================

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }

    // ================================================================================

    double             getTailLengthSeconds() const override { return 0.0; }
    int                getNumPrograms() override { return 1; }
    int                getCurrentProgram() override { return 0; }
    const juce::String getProgramName ([[maybe_unused]] int index) override { return "default"; }
    void               setCurrentProgram ([[maybe_unused]] int index) override { }
    void               changeProgramName ([[maybe_unused]] int index, [[maybe_unused]] const juce::String& newName) override { }

    // ================================================================================

private:
    foleys::License license;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LicensingDemoProcessor)
};


#endif  // FOLEYS_LICENSING_CLIENT_LICENSINGDEMOPROCESSOR_H
