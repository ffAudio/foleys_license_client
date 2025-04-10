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
        ../foleys_license_common/foleys_License.cpp
        ../foleys_license_common/foleys_License.h
        ../foleys_license_common/foleys_LicenseDefines.h
        ../foleys_license_common/foleys_Crypto.cpp
        ../foleys_license_common/foleys_Crypto.h
        foleys_Observers.h
        foleys_LicenseUpdater.cpp
        foleys_LicenseUpdater.h
        foleys_SharedObject.h
        foleys_SystemInfo.h
        foleys_SystemInfo.cpp
        choc_Base64.h)

if (PREBUILT_SODIUM)
    if (APPLE)
        set(SODIUM_LIB ${CMAKE_CURRENT_LIST_DIR}/../foleys_license_common/libsodium/macos/libsodium.a)
    else ()
        set(SODIUM_LIB ${CMAKE_CURRENT_LIST_DIR}/../foleys_license_common/libsodium/msvc-142/libsodium.lib)
    endif ()
else ()
    include(Sodium)
    set(SODIUM_LIB sodium)
endif ()

if (APPLE)
    target_sources(licensing
            INTERFACE
            foleys_NetworkRequest.mm
            foleys_SystemInfo.mm)

    target_link_libraries(licensing INTERFACE
            "-framework Foundation")
else ()
    target_sources(licensing
            INTERFACE
            foleys_NetworkRequest.cpp
            foleys_SystemInfo.cpp)
endif ()

target_link_libraries(licensing
        INTERFACE
        ${SODIUM_LIB}
        sago::platform_folders)

target_include_directories(licensing
        INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/..
        ${nlohmann_SOURCE_DIR}/include
        ${filesystem_SOURCE_DIR}/include)
