cmake_minimum_required(VERSION 3.24.1 FATAL_ERROR)

# ================================================================================
# foleys_license_client juce plugin example
# ================================================================================


set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14)
option(FOLEYS_LICENSING_HAS_JUCE "Enable juce GUI for the license client" ON)

include(juce)

juce_add_plugin(FoleysJuceLicenseExample
        PRODUCT_NAME LicenseExample
        COMPANY_NAME Manufacturer
        PLUGIN_MANUFACTURER_CODE Manu
        PLUGIN_CODE Lics
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        FORMATS AU VST3 Standalone
        BUNDLE_ID com.example.foleyslicenseexample)

target_sources(FoleysJuceLicenseExample
        PRIVATE
        LicensingDemoProcessor.cpp
        LicensingDemoProcessor.h
        LicensingDemoEditor.cpp
        LicensingDemoEditor.h
        LicensePanel.cpp
        LicensePanel.h
        LicenseDeactivate.cpp
        LicenseDeactivate.h)

target_sources(licensing
        INTERFACE
        LicenseData.cpp)

target_compile_definitions(FoleysJuceLicenseExample
        PRIVATE
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

juce_add_binary_data(
        FoleysJuceLicenseExample_data
        SOURCES
        Resources/close-icon.svg
        Resources/refresh-icon.svg
        Resources/pdf-icon.svg
        Resources/key-icon.svg
        Resources/www-icon.svg)

target_link_libraries(FoleysJuceLicenseExample
        PRIVATE
        FoleysJuceLicenseExample_data
        licensing
        juce::juce_audio_processors
        juce::juce_audio_devices
        juce::juce_audio_utils
        juce::juce_recommended_warning_flags)
