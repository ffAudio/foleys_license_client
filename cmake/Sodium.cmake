cmake_minimum_required(VERSION 3.24.1 FATAL_ERROR)

# ================================================================================
# libsodium and CMake build
# ================================================================================


# decryption
add_library(sodium STATIC IMPORTED GLOBAL)

include(ExternalProject)

set(SODIUM_ROOT ${CMAKE_BINARY_DIR}/_deps/libsodium-src)
set(SODIUM_PATH ${SODIUM_ROOT}/src/libsodium)

if(APPLE)
    set(SODIUM_BUILD_SCRIPT ${SODIUM_PATH}/dist-build/macos.sh)
    set(SODIUM_INCLUDE_PATH ${SODIUM_PATH}/libsodium-osx/include)
    set(SODIUM_LIBRARY_FILE ${SODIUM_PATH}/libsodium-osx/lib/libsodium.a)
else()
    set(SODIUM_BUILD_SCRIPT ${CMAKE_COMMAND} -E msbuild ${SODIUM_PATH}/builds/msvc/vs2022/libsodium/libsodium.sln /property:Configuration=${CMAKE_BUILD_TYPE})
    set(SODIUM_INCLUDE_PATH ${SODIUM_PATH}/libsodium-win/include)
    set(SODIUM_LIBRARY_FILE ${SODIUM_PATH}/libsodium-win/lib/sodium.lib)
endif()

ExternalProject_Add(libsodium
        PREFIX ${SODIUM_ROOT}
        URL https://download.libsodium.org/libsodium/releases/LATEST.tar.gz
        CONFIGURE_COMMAND ""
        BUILD_IN_SOURCE TRUE
        BUILD_COMMAND ${SODIUM_BUILD_SCRIPT}
        BUILD_BYPRODUCTS ${SODIUM_LIBRARY_FILE}
        INSTALL_COMMAND ""
)


add_dependencies(sodium libsodium)

set_property(TARGET sodium PROPERTY
        IMPORTED_LOCATION "${SODIUM_LIBRARY_FILE}")

target_include_directories(sodium INTERFACE "${SODIUM_INCLUDE_PATH}")
