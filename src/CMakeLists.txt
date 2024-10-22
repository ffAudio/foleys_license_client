cmake_minimum_required(VERSION 3.24.1 FATAL_ERROR)

# ================================================================================
# foleys_license_client implementation
# ================================================================================

include(FetchContent)

include(CPR)
include(nlohmann)
include(Sodium)

add_library(licensing
        INTERFACE)

target_sources(licensing
        INTERFACE
        foleys_License.cpp
        foleys_License.h
        private/foleys_Licensing.h
        private/foleys_LicenseUpdater.cpp
        private/foleys_LicenseUpdater.h
        private/foleys_Crypto.cpp
        private/foleys_Crypto.h
        private/foleys_SharedObject.h
        private/foleys_Observers.h
        private/foleys_SystemInfo.h
        private/choc_Base64.h)

option(FOLEYS_LICENSING_HAS_JUCE "Set this ON if you build a juce project. juce needs be present in the including target." OFF)

if (FOLEYS_LICENSING_HAS_JUCE)
    target_sources(licensing
            INTERFACE
            juce/foleys_JuceUtilities.h
            juce/foleys_PopupHolder.h)
endif ()


if (APPLE)
    target_sources(licensing INTERFACE private/foleys_SystemInfo.mm)
else ()
    target_sources(licensing INTERFACE private/foleys_SystemInfo.cpp)
endif ()


target_link_libraries(licensing
        INTERFACE
        cpr::cpr
        sodium)

target_include_directories(licensing
        INTERFACE
        ${nlohmann_SOURCE_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR})
