cmake_minimum_required(VERSION 3.24.1 FATAL_ERROR)
#[[
================================================================================
foleys_license_client

Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************
]]

if (NOT DEFINED CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Architectures" FORCE)
endif ()

if (NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET)
    if (FOLEYS_LICENSING_HAS_JUCE)
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14)
    else ()
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)
    endif ()
endif ()

# various examples
option(FOLEYS_LICENSING_BUILD_EXAMPLES "Build the examples." ON)
option(FOLEYS_LICENSING_BUILD_EXAMPLES_AAX "Build the examples with AAX version." OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(FetchContent)

project(foleys_licensing_client
        VERSION 0.1.0)

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif ()

# Best leave this as it is to use the bundled sodium library
option(PREBUILT_SODIUM "Use the prebuilt sodium library" ON)

# Set this to ON if you build a JUCE product
option(FOLEYS_LICENSING_HAS_JUCE "Use JUCE functions and bindings" OFF)

if (FOLEYS_LICENSING_HAS_JUCE)
    include(juce)
    if (FOLEYS_LICENSING_BUILD_EXAMPLES)
        add_subdirectory(examples/juce_plugin)
    endif ()
    juce_add_module(foleys_license_juce ALIAS_NAMESPACE foleys)
    target_compile_definitions(foleys_license_juce INTERFACE FOLEYS_LICENSING_HAS_JUCE=1)

    # TODO: can this be added into the juce module?
    if (APPLE)
        target_link_directories(foleys_license_juce INTERFACE "${CMAKE_CURRENT_LIST_DIR}/foleys_license_common/libsodium/macos")
    else ()
        target_link_directories(foleys_license_juce INTERFACE "${CMAKE_CURRENT_LIST_DIR}/foleys_license_common/libsodium/msvc-143")
    endif ()

else ()
    # Don't use JUCE functions
    add_subdirectory(foleys_license_lib)

    if (FOLEYS_LICENSING_BUILD_EXAMPLES)
        add_subdirectory(examples/minimal)
    endif ()
endif ()

# unit tests
option(FOLEYS_LICENSING_BUILD_TESTS "Build and run the tests." OFF)

if (FOLEYS_LICENSING_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()
