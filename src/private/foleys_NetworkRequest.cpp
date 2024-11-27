/********************************************************************************
    Copyright 2024 - Daniel Walz
    Foleys Finest Audio UG (haftungsbeschraenkt)
    Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

    This code is provided under the ISC license

    This code is provided as is. The authors disclaim all warranties.
    For details refer to the LICENSE.md

*******************************************************************************/

#if (WIN32)

    #include "foleys_NetworkRequest.h"
    #include <windows.h>
    #include <winhttp.h>
    #include <stdexcept>
    #include <winhttp.h>
    #include <thread>
    #include <chrono>
    #include <functional>
    #include <iostream>
    #include <cassert>
    #include <sstream>

    #pragma comment(lib, "winhttp.lib")

    #define ASSERT_ON_FAULT 1

inline void debugLog (const std::string& message, bool toCerr = false)
{
    if (toCerr)
    {

        std::cerr << message << std::endl;
    }
    else
    {
        std::cout << message << std::endl;
    }

    // Log to Visual Studio Debug Output.
    OutputDebugStringA (message.c_str());
    OutputDebugStringA ("\n");

    #if ASSERT_ON_FAULT
    assert (!toCerr);
    #endif
}


    #define DBUG(...)                    \
        do                               \
        {                                \
            std::ostringstream oss;      \
            oss << __VA_ARGS__;          \
            debugLog (oss.str(), false); \
        } while (false)

    #define FAULT(...)                  \
        do                              \
        {                               \
            std::ostringstream oss;     \
            oss << __VA_ARGS__;         \
            debugLog (oss.str(), true); \
        } while (false)


/** @brief Helper function to convert error code to a string. */
std::string getErrorMessage (DWORD errorCode)
{
    char*   errorText = nullptr;
    HMODULE winHttp   = LoadLibraryA ("winhttp.dll");  // Load the WinHTTP module
    DWORD   flags     = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    if (winHttp)
        flags |= FORMAT_MESSAGE_FROM_HMODULE;  // Add this flag to query WinHTTP-specific errors

    DWORD length = FormatMessageA (flags, winHttp, errorCode, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &errorText, 0, nullptr);

    if (winHttp)
        FreeLibrary (winHttp);

    // FormatMessage failed to find the message.
    if (length == 0)
    {
        std::ostringstream oss;
        oss << "Unknown error (code " << errorCode << ")";
        return oss.str();
    }

    std::string errorMessage (errorText);
    LocalFree (errorText);

    return errorMessage;
}


class PerformOnExit
{
public:
    PerformOnExit() = default;

    explicit PerformOnExit (std::function<void()> callbackFn) : m_callbackFn (std::move (callbackFn)) { }

    ~PerformOnExit() noexcept
    {
        if (auto d = std::exchange (m_callbackFn, nullptr))
            d();
    }

private:
    std::function<void()> m_callbackFn;
    FOLEYS_DISABLE_COPY (PerformOnExit)
};


namespace foleys
{


NetworkRequest::NetworkRequest (std::string_view urlToAccess) : url (urlToAccess) { }

NetworkRequest::~NetworkRequest()
{
    cancel();
}

std::wstring NetworkRequest::stringToWString (const std::string& str)
{
    // Convert UTF-8 string to wide string (UTF-16) using MultiByteToWideChar
    int          size_needed = MultiByteToWideChar (CP_UTF8, 0, str.c_str(), static_cast<int> (str.size()), nullptr, 0);
    std::wstring wstr (size_needed, 0);
    MultiByteToWideChar (CP_UTF8, 0, str.c_str(), static_cast<int> (str.size()), &wstr[0], size_needed);
    return wstr;
}


void NetworkRequest::splitUrl (const std::string& fullUrl, std::wstring& protocol, std::wstring& domain, std::wstring& path)
{
    // Splitting protocol...
    const auto protocol_end = fullUrl.find ("://");

    std::string narrowProtocol = "";
    if (protocol_end != std::string::npos)
        narrowProtocol = std::string_view (fullUrl.data(), protocol_end + 3);
    protocol = NetworkRequest::stringToWString (narrowProtocol);

    // Splitting domain and path...
    std::string narrowPath   = "";
    std::string narrowDomain = "";
    const auto  domain_start = protocol_end == std::string::npos ? 0 : protocol_end + 3;
    const auto  domain_end   = fullUrl.find ('/', domain_start);
    if (domain_end != std::string::npos)
    {
        narrowDomain = std::string_view (fullUrl.data() + domain_start, domain_end - domain_start);
        narrowPath   = std::string_view (fullUrl.data() + domain_end, fullUrl.size() - domain_end);
    }
    else
    {
        narrowDomain = std::string_view (fullUrl.data() + domain_start, fullUrl.size() - domain_start);
    }

    domain = NetworkRequest::stringToWString (narrowDomain);
    path   = NetworkRequest::stringToWString (narrowPath);
}

void CALLBACK httpCallback (HINTERNET internet, DWORD_PTR context, DWORD internetStatus, LPVOID statusInformation, DWORD statusInfoLength)
{
    std::vector<char> responseBuffer;

    auto networkRequest = (NetworkRequest*) context;

    // switch (internetStatus)
    //{
    //     case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
    //     {
    //         DBUG ("Request sent. Now reading response..." << "\r\n");
    //         break;
    //     }
    //     case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
    //     {
    //         auto* error = reinterpret_cast<WINHTTP_ASYNC_RESULT*> (statusInformation);
    //         FAULT ("Request error: " << getErrorMessage (error->dwError) << ".");
    //         break;
    //     }
    //     case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
    //     {
    //         // When the read operation is complete, the data will be in lpvStatusInformation
    //         DWORD bytesRead = statusInfoLength;
    //         if (bytesRead > 0)
    //         {
    //             responseBuffer.insert (responseBuffer.end(), static_cast<char*> (statusInformation), static_cast<char*> (statusInformation) + bytesRead);
    //             std::cout << "Read " << bytesRead << " bytes." << std::endl;
    //         }

    //        // If there is more data to read, continue reading
    //        DWORD dwSize       = 0;
    //        DWORD dwDownloaded = 0;
    //        if (WinHttpQueryDataAvailable (request, &dwSize) && dwSize > 0)
    //        {
    //            char* buffer = new char[dwSize + 1];
    //            ZeroMemory (buffer, dwSize + 1);

    //            // Start the read operation again
    //            if (WinHttpReadData (hRequest, (LPVOID) buffer, dwSize, &dwDownloaded))
    //            {
    //                // Pass the data to the callback function (this will happen when data is read)
    //                MyCallback (internet, dwContext, WINHTTP_CALLBACK_STATUS_READ_COMPLETE, buffer, dwDownloaded);
    //            }
    //            else
    //            {
    //                std::cerr << "Failed to read data. Error: " << GetLastError() << std::endl;
    //            }
    //            delete[] buffer;
    //        }
    //        else
    //        {
    //            std::cout << "No more data available." << std::endl;
    //            // End of the response data, process the full response
    //            std::cout << "Complete response: " << std::string (responseBuffer.begin(), responseBuffer.end()) << std::endl;
    //        }
    //        break;
    //    }
    //    case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
    //    {
    //        DBUG ("Headers received.");
    //        break;
    //    }
    //    case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
    //    {
    //        auto* bytesAvailable = reinterpret_cast<DWORD*> (statusInformation);
    //        DBUG ("Data available: " << *bytesAvailable << " bytes.");
    //        if (*bytesAvailable > 0)
    //        {
    //            std::vector<char> buffer (*bytesAvailable);
    //            DWORD             bytesRead = 0;
    //            if (WinHttpReadData (internet, buffer.data(), *bytesAvailable, &bytesRead))
    //                DBUG ("Read " << bytesRead << " bytes: " << std::string (buffer.begin(), buffer.begin() + bytesRead) << "\r\n");
    //            else
    //                FAULT ("Failed to read data: " << getErrorMessage (GetLastError()) << "\r\n");
    //        }
    //        break;
    //    }
    //    case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
    //    {
    //        DBUG ("Read complete." << "\r\n");
    //        break;
    //    }
    //    default: DBUG ("Unhandled internetStatus: " << internetStatus << "\r\n");
    //}
}


void NetworkRequest::fetch (std::string_view payload, bool async /*= true*/, bool post /*= true*/)
{
    HINTERNET session = nullptr;
    HINTERNET connect = nullptr;
    HINTERNET request = nullptr;

    const PerformOnExit exitGuard { [&]
                                    {
                                        const auto error = GetLastError();

                                        if (session)
                                            WinHttpCloseHandle (session);
                                        if (connect)
                                            WinHttpCloseHandle (connect);
                                        if (request)
                                            WinHttpCloseHandle (request);

                                        if (error != 0)
                                            FAULT ("Problem fetching license. Error: " << getErrorMessage (error) << "!");

                                        return;
                                    } };

    // Initializing WinHTTP session...
    if (!(session = WinHttpOpen (kUserAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, async ? WINHTTP_FLAG_ASYNC : 0)))
        return;

    // Preparing the url...
    std::wstring protocol, domain, path;
    NetworkRequest::splitUrl (url, protocol, domain, path);

    // Connecting to the server...
    if (!(connect = WinHttpConnect (session, domain.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0)))
        return;

    // Creating HTTP request....
    if (!(request = WinHttpOpenRequest (connect, post ? L"POST" : L"GET",  // HTTP method
                                        path.c_str(),                      // Request URI
                                        nullptr,                           // No additional headers (optional)
                                        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE)))
        return;

    // Setting callback for asynchronous events...
    if (async)
    {
        if (WinHttpSetStatusCallback (request, httpCallback, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, 0) == WINHTTP_INVALID_STATUS_CALLBACK)
        {
            FAULT ("Failed to set callback: " << getErrorMessage (GetLastError()) << "\r\n");
            return;
        }
    }

    // Creating headers...
    const auto   data    = std::string (payload);
    std::wstring headers = L"Content-Type: application/json\r\nContent-Length: " + std::to_wstring (data.size()) + L"\r\n";

    // Sending the request...
    BOOL results;
    if (!(results = WinHttpSendRequest (request,
                                        headers.c_str(),        // Set content type header.
                                        (DWORD) -1L,            // Automatically calculate the header size.
                                        (LPVOID) data.c_str(),  // Send the data with the request.
                                        (DWORD) data.length(),  // Length of the data.
                                        0,                      // No extra data.
                                        (DWORD_PTR) this)))     // No extra data context.
        return;

    if (async)
        return;

    // Writing the body data...
    DWORD bytesWritten = 0;
    if (!(WinHttpWriteData (request, data.data(), (DWORD) data.size(), &bytesWritten)))
        return;

    if (bytesWritten != data.size())
    {
        FAULT ("Incomplete body sent. Bytes written: " << std::to_string (bytesWritten) << "\r\n");
        return;
    }

    // Waiting for the response...
    results = WinHttpReceiveResponse (request, nullptr);
    if (!results)
        return;

    // Query the status code
    wchar_t statusCodeBuffer[16] = {};
    DWORD   bufferLength         = sizeof (statusCodeBuffer);
    if (!(WinHttpQueryHeaders (request, WINHTTP_QUERY_STATUS_CODE, WINHTTP_HEADER_NAME_BY_INDEX, statusCodeBuffer, &bufferLength, WINHTTP_NO_HEADER_INDEX)))
        return;

    int statusCode = std::wcstol (statusCodeBuffer, nullptr, 10);

    // Reading the response from the server...
    DWORD       size          = 0;
    DWORD       numDownloaded = 0;
    std::string response;
    do
    {
        // Checking the size of the available data...
        WinHttpQueryDataAvailable (request, &size);
        if (size == 0)
            break;

        // Reading...
        std::vector<char> buffer (size);
        WinHttpReadData (request, buffer.data(), size, &numDownloaded);
        response.append (buffer.data(), numDownloaded);

    } while (size > 0);

    if (statusCode < 300 && callback)
        callback (statusCode, response);
}

void NetworkRequest::cancel() { }

}  // namespace foleys

#endif