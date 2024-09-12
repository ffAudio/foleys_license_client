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
    set(SODIUM_LIBRARY_FILE ${SODIUM_PATH}/libsodium-osx/lib/libsodium.a)

    ExternalProject_Add(libsodium
        PREFIX ${SODIUM_ROOT}
        URL https://download.libsodium.org/libsodium/releases/LATEST.tar.gz
        CONFIGURE_COMMAND ""
        BUILD_IN_SOURCE TRUE
        BUILD_COMMAND ${SODIUM_PATH}/dist-build/macos.sh
        BUILD_BYPRODUCTS ${SODIUM_LIBRARY_FILE}
        INSTALL_COMMAND ""
    )

    add_dependencies(sodium libsodium)
    set_property(TARGET sodium PROPERTY IMPORTED_LOCATION "${SODIUM_LIBRARY_FILE}")
    target_include_directories(sodium INTERFACE "${SODIUM_PATH}/libsodium-osx/include")

else()

    FetchContent_Declare(download_sodium
            URL https://download.libsodium.org/libsodium/releases/libsodium-1.0.20-stable-msvc.zip)

    FetchContent_MakeAvailable(download_sodium)

    add_dependencies(sodium download_sodium)

    target_compile_definitions(sodium INTERFACE SODIUM_STATIC)
    target_include_directories(sodium INTERFACE "${download_sodium_SOURCE_DIR}/include")
    set_property(TARGET sodium PROPERTY
            IMPORTED_LOCATION "${download_sodium_SOURCE_DIR}/x64/${CMAKE_BUILD_TYPE}/v143/static/libsodium.lib")

endif()
