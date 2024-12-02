#[[

 Adding this include will add nlohmann JSON to your project

]]

FetchContent_Declare(nlohmann
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_SHALLOW ON)
FetchContent_MakeAvailable(nlohmann)
