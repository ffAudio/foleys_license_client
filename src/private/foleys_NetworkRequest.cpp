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
    #include <iostream>

    #pragma comment(lib, "winhttp.lib")


namespace foleys
{


/** @brief Helper function to convert error code to a string. */
std::string getErrorMessage (DWORD errorCode)
{
    char* errorText = nullptr;
    FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode, 0, (LPSTR) &errorText, 0, nullptr);
    std::string errorMessage (errorText);
    LocalFree (errorText);
    return errorMessage;
}


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

void NetworkRequest::fetch (std::string_view payload)
{
    cancel();

    // Initializing WinHTTP session...
    auto session = WinHttpOpen (L"foleys_license/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session)
    {
        const auto error = GetLastError();
        std::cerr << "Failed to open WinHTTP session. Error: " << getErrorMessage (error) << std::endl;
        return;
    }

    // Preparing the url...
    std::wstring protocol, domain, path;
    NetworkRequest::splitUrl (url, protocol, domain, path);

    // Connecting to the server...
    const auto connect = WinHttpConnect (session, domain.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);
    if (!connect)
    {
        const auto error = GetLastError();
        std::cerr << "Failed to connect to server. Error: " << getErrorMessage (error) << std::endl;
        WinHttpCloseHandle (session);
        return;
    }

    // Creating HTTP request....
    const auto request = WinHttpOpenRequest (connect, L"POST", path.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!request)
    {
        const auto error = GetLastError();
        std::cerr << "Failed to open HTTP request. Error: " << getErrorMessage (error) << std::endl;
        WinHttpCloseHandle (connect);
        WinHttpCloseHandle (session);
        return;
    }

    // Sending the request...
    const auto payloadW = NetworkRequest::stringToWString (std::string (payload));
    auto       results  = WinHttpSendRequest (request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (LPVOID) payloadW.c_str(), static_cast<DWORD> (payloadW.size()),
                                              static_cast<DWORD> (payloadW.size()), 0);
    if (!results)
    {
        const auto error = GetLastError();
        std::cerr << "Failed to send HTTP request. Error: " << getErrorMessage (error) << std::endl;
        WinHttpCloseHandle (request);
        WinHttpCloseHandle (connect);
        WinHttpCloseHandle (session);
        return;
    }

    // Waiting for the response...
    results = WinHttpReceiveResponse (request, NULL);
    if (!results)
    {
        const auto error = GetLastError();
        std::cerr << "Failed to receive HTTP response. Error: " << error << std::endl;
        WinHttpCloseHandle (request);
        WinHttpCloseHandle (connect);
        WinHttpCloseHandle (session);
        return;
    }

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

    // Clean up...
    // TODO: - [mh] Make ScopeGuard.
    WinHttpCloseHandle (request);
    WinHttpCloseHandle (connect);
    WinHttpCloseHandle (session);
}

void NetworkRequest::cancel()
{
    // at least avoid entering the callback
    callback = nullptr;
}

}  // namespace foleys

#endif