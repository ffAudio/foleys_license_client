#[[

 Adding this include will add GHC as std::filesystem replacement to your project

]]

option(GHC_FILESYSTEM_BUILD_EXAMPLES "Build examples" OFF)
option(GHC_FILESYSTEM_BUILD_TESTING "Enable tests" OFF)
option(GHC_FILESYSTEM_WITH_INSTALL "With install target" OFF)

FetchContent_Declare(filesystem
        GIT_REPOSITORY https://github.com/gulrak/filesystem.git
        GIT_SHALLOW ON)
FetchContent_MakeAvailable(filesystem)
