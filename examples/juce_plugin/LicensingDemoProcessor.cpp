//
// Created by Daniel Walz on 12.08.24.
//

#include "LicensingDemoProcessor.h"
#include "LicensingDemoEditor.h"

#include <juce_events/juce_events.h>

LicensingDemoProcessor::LicensingDemoProcessor()
{
    auto licenseFile = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
#if JUCE_MAC
                         .getChildFile ("Application Support")
#endif
                         .getChildFile ("Manufacturer")
                         .getChildFile (LicenseData::productName)
                         .withFileExtension (".lic");

    license.setupLicenseData (licenseFile.getFullPathName().toStdString(), juce::SystemStats::getUniqueDeviceID().toRawUTF8(),
                              { { LicenseID::version, LicenseData::version },
                                { LicenseID::hardware, juce::SystemStats::getUniqueDeviceID().toRawUTF8() },
                                { LicenseID::os, juce::SystemStats::getOperatingSystemName().toRawUTF8() },
                                { LicenseID::host, juce::JUCEApplicationBase::isStandaloneApp() ? "Standalone" : juce::PluginHostType().getHostDescription() } });

    license.onLicenseReceived = [this]
    {
        DBG ("License received: " + license.getRawLicenseData());
    };
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
