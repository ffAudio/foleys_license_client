Foleys License Client
=====================

This is a C++ client to communicate with the Foleys License Server.


Disclaimer
----------

THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.


Adding
------

Use the following in your cmake file:
```cmake
option(FOLEYS_LICENSING_BUILD_EXAMPLES "Build and run the tests." OFF)
option(FOLEYS_LICENSING_BUILD_EXAMPLES "Build the examples." OFF)
option(FOLEYS_LICENSING_HAS_JUCE "Enable juce GUI for the license client" ON)

FetchContent_Declare(license
        GIT_REPOSITORY https://github.com/ffAudio/foleys_license_client.git
        GIT_TAG main
        GIT_SHALLOW ON)
FetchContent_MakeAvailable(license)
```

Since this project uses objective-C on mac, you need to enable OBJXX in your top level CMakeLists.txt:
```cmake
if (APPLE)
    enable_language(OBJCXX)
endif()
```

Once you created a product in the website you also create a Version there, which has it's own unique key pair to talk to the server.
Also the version number will show up in the usage statistics.



