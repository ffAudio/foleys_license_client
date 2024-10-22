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

set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Architectures" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(foleys_licensing_client
        VERSION 0.0.1)

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif ()

option(PREBUILT_SODIUM "Use the prebuilt sodium library" ON)

if (APPLE)
    enable_language(OBJCXX)
endif()

# src contains the actual library
add_subdirectory(src)

# unit tests
option(FOLEYS_LICENSING_BUILD_TESTS "Build and run the tests." OFF)

if (FOLEYS_LICENSING_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

# various examples
option(FOLEYS_LICENSING_BUILD_EXAMPLES "Build the examples." ON)

if (FOLEYS_LICENSING_BUILD_EXAMPLES)
    add_subdirectory(examples/juce_plugin)
    add_subdirectory(examples/minimal)
endif ()