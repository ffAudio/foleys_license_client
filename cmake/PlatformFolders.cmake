#[[

 Adding this include will add GHC as std::filesystem replacement to your project

]]


FetchContent_Declare(platform_folders
        GIT_REPOSITORY https://github.com/sago007/PlatformFolders.git
        GIT_SHALLOW ON)
FetchContent_MakeAvailable(platform_folders)


