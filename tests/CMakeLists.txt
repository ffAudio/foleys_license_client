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

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_PROGRESS TRUE
        GIT_TAG origin/devel
        GIT_SHALLOW ON)

FetchContent_MakeAvailable(Catch2)

if (catch2_SOURCE_DIR)
    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
else ()
    message(WARNING "The variable catch2_SOURCE_DIR was not set, including the Catch CMake module may fail in a moment...")
endif ()


