//
// Created by Daniel Walz on 12.08.24.
//

#include "LicensingDemoProcessor.h"
#include "LicensingDemoEditor.h"


LicensingDemoProcessor::LicensingDemoProcessor()
{
    license.setupLicenseData (foleys::SystemInfo::createLicensePath ("Manufacturer", LicenseData::productName), juce::SystemStats::getUniqueDeviceID().toRawUTF8(),
                              { { LicenseID::version, LicenseData::version },
                                { LicenseID::hardware, juce::SystemStats::getUniqueDeviceID().toRawUTF8() },
                                { LicenseID::os, juce::SystemStats::getOperatingSystemName().toRawUTF8() },
                                { LicenseID::host, juce::JUCEApplicationBase::isStandaloneApp() ?
                                                     "Standalone" :
                                                     juce::PluginHostType().getHostDescription() } });
}

void LicensingDemoProcessor::prepareToPlay ([[maybe_unused]] double sampleRate, [[maybe_unused]] int expectedNumSamples) { }

void LicensingDemoProcessor::processBlock (juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midi)
{
    if (!license.isAllowed())
    {
        buffer.clear();
        return;
    }

    for (int c = getTotalNumInputChannels(); c < getTotalNumOutputChannels(); ++c)
        buffer.clear (c, 0, buffer.getNumSamples());
}

void LicensingDemoProcessor::releaseResources() { }

juce::AudioProcessorEditor* LicensingDemoProcessor::createEditor()
{
    return new LicensingDemoEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LicensingDemoProcessor();
}
