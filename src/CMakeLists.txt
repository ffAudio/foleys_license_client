cmake_minimum_required(VERSION 3.24.1 FATAL_ERROR)

# ================================================================================
# foleys_license_client implementation
# ================================================================================

include(FetchContent)

# cpr is a wrapper around libcurl
option(CURL_ZLIB OFF)
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
        private/foleys_SharedObject.h
        private/foleys_Observers.h
        private/foleys_SystemInfo.h
        private/choc_Base64.h)

if(APPLE)
    target_sources(licensing INTERFACE private/foleys_SystemInfo.mm)
else()
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
