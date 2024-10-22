#[[

 Adding this include will add CPR to your project

]]


# cpr is a wrapper around libcurl
option(CURL_ZLIB "No zlib" OFF)
option(BUILD_CURL_EXE "No exe" OFF)
option(BUILD_SHARED_LIBS "No shared libs" OFF)
option(BUILD_STATIC_LIBS "Static libs please" ON)
option(ENABLE_CURL_MANUAL "No manual" OFF)
option(BUILD_LIBCURL_DOCS "No docs" OFF)
option(CURL_USE_LIBPSL "No psl" OFF)
option(USE_LIBIDN2 "No IDN" OFF)
option(USE_NGHTTP2 "No NGHTTP2" OFF)
option(CURL_USE_LIBSSH2 "No SSH2" OFF)

option(CPR_BUILD_TESTS "No tests" OFF)

FetchContent_Declare(cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_SHALLOW ON)
FetchContent_MakeAvailable(cpr)
