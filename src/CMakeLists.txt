cmake_minimum_required(VERSION 3.24.1 FATAL_ERROR)

# ================================================================================
# foleys_license_client implementation
# ================================================================================

option(FOLEYS_LICENSING_HAS_JUCE "Set this ON if you build a juce project. juce needs be present in the including target." ON)

include(FetchContent)

include(nlohmann)
include(GHC-filesystem)
include(PlatformFolders)

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
        private/foleys_SystemInfo.cpp
        private/choc_Base64.h)

if (PREBUILT_SODIUM)
    if (APPLE)
        set(SODIUM_LIB ${CMAKE_CURRENT_LIST_DIR}/../libsodium/macos/libsodium.a)
    else ()
        set(SODIUM_LIB ${CMAKE_CURRENT_LIST_DIR}/../libsodium/msvc-142/libsodium.lib)
        target_compile_definitions(licensing INTERFACE SODIUM_STATIC)
    endif ()
else ()
    include(Sodium)
    set(SODIUM_LIB sodium)
endif ()

if (APPLE)
    target_sources(licensing
            INTERFACE
            private/foleys_NetworkRequest.mm
            private/foleys_SystemInfo.mm)

    target_link_libraries(licensing INTERFACE
            "-framework Foundation")
else ()
    target_sources(licensing
            INTERFACE
            private/foleys_NetworkRequest.cpp
            private/foleys_SystemInfo.cpp)
endif ()

target_link_libraries(licensing
        INTERFACE
        ${SODIUM_LIB}
        sago::platform_folders)

target_include_directories(licensing
        INTERFACE
        ${nlohmann_SOURCE_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/../libsodium/include
        ${filesystem_SOURCE_DIR}/include
#        ${platformfolders_SOURCE_DIR}/sago
        ${CMAKE_CURRENT_LIST_DIR})
