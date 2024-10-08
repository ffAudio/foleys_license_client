cmake_minimum_required(VERSION 3.24.1 FATAL_ERROR)

# ================================================================================
# foleys_license_client implementation
# ================================================================================

include(FetchContent)

# cpr is a wrapper around libcurl
option(CURL_ZLIB OFF)
option(BUILD_CURL_EXE OFF)
option(BUILD_SHARED_LIBS OFF)
option(BUILD_STATIC_LIBS ON)
option(CURL_DISABLE_INSTALL ON)
option(ENABLE_CURL_MANUAL OFF)
option(BUILD_LIBCURL_DOCS OFF)
option(CURL_USE_LIBPSL OFF)
option(USE_LIBIDN2 OFF)
option(USE_NGHTTP2 OFF)
option(CURL_USE_LIBSSH2 OFF)

option(CPR_BUILD_TESTS OFF)
FetchContent_Declare(cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_SHALLOW ON)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(nlohmann
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_SHALLOW ON)
FetchContent_MakeAvailable(nlohmann)

include(Sodium)

add_library(licensing
        INTERFACE)

target_sources(licensing
        INTERFACE
        foleys_License.cpp
        foleys_License.h
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
