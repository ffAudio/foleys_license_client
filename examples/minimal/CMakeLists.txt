cmake_minimum_required(VERSION 3.24.1 FATAL_ERROR)

# ================================================================================
# foleys_license_client juce plugin example
# ================================================================================



add_executable(FoleysLicenseCLI)

target_sources(FoleysLicenseCLI
        PRIVATE
        main.cpp
        LicenseData.cpp
)

target_link_libraries(FoleysLicenseCLI
        PRIVATE
        licensing)
