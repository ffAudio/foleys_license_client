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
    #include <functional>
    #include <iostream>
    #include <cassert>
    #include <sstream>

    #pragma comment(lib, "winhttp.lib")

    #define ASSERT_ON_FAULT 1

inline void debugLog (const std::string& message, bool toCerr = false)
{
    if (toCerr)
        std::cerr << message << std::endl;
    else
        std::cout << message << std::endl;

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


namespace foleys
{


void CALLBACK httpCallback (HINTERNET internet, DWORD_PTR context, DWORD internetStatus, LPVOID statusInformation, DWORD statusInfoLength)
{
    std::vector<char> responseBuffer;
    NetworkRequest*   requestOwner = reinterpret_cast<NetworkRequest*> (context);

    switch (internetStatus)
    {
        // 1.
        case WINHTTP_CALLBACK_STATUS_RESOLVING_NAME: DBUG ("1. Resolving name..."); break;
        // 2.
        case WINHTTP_CALLBACK_STATUS_NAME_RESOLVED: DBUG ("2. Name resolved."); break;
        // 3.
        case WINHTTP_CALLBACK_STATUS_CONNECTING_TO_SERVER: DBUG ("3. Connecting to server..."); break;
        // 4.
        case WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER: DBUG ("4. Connected to server."); break;
        // 5.
        case WINHTTP_CALLBACK_STATUS_SENDING_REQUEST: DBUG ("5. Sending request..."); break;
        // 6.
        case WINHTTP_CALLBACK_STATUS_REQUEST_SENT: DBUG ("6. Request sent. Waiting for response..."); break;
        // 7.
        case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
        {
            DBUG ("7. Send request complete. Sending payload...");

            // Sending the payload data when the request has been prepared...
            const auto& data = requestOwner->getPayload();
            if (!data.empty())
            {
                DWORD bytesWritten = 0;
                if (!WinHttpWriteData (internet, data.c_str(), static_cast<DWORD> (data.length()), &bytesWritten))
                    FAULT ("Failed to write data. Error: " << getErrorMessage (GetLastError()));
            }
            break;
        }
        // 8.
        case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
        {
            DBUG ("8. Writing payload completed.");

            // Data successfully written, start reading the response...
            if (!WinHttpReceiveResponse (internet, nullptr))
                DBUG ("Failed to receive response. Error: " << getErrorMessage (GetLastError()));

            break;
        }
        // 9.
        case WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE: DBUG ("9. Receiving response..."); break;
        // 10.
        case WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED:
        {
            // This indicates that headers and response data are available.
            // Start querying the headers or reading the response body.
            DBUG ("10. Response headers received, starting to read response body...");

            DWORD size = 0;
            if (WinHttpQueryDataAvailable (internet, &size) && size > 0)
            {
                // Trigger a read operation to fetch the response data.
                auto*             bytesAvailable = reinterpret_cast<DWORD*> (statusInformation);
                std::vector<char> buffer (*bytesAvailable);
                DWORD             bytesRead = 0;
                if (!WinHttpReadData (internet, buffer.data(), size, &bytesRead))
                    FAULT ("Failed to read data. Error: " << getErrorMessage (GetLastError()));
            }
            break;
        }
        // 11.
        case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
        {
            DBUG ("11. Headers available...");

            DWORD size = 0;
            if (WinHttpQueryDataAvailable (internet, &size) && size > 0)
            {
                if (requestOwner->hasReceivedResponse())
                    break;

                auto*             bytesAvailable = reinterpret_cast<DWORD*> (statusInformation);
                std::vector<char> buffer (*bytesAvailable);
                DWORD             bytesRead = 0;
                if (!WinHttpReadData (internet, buffer.data(), size, &bytesRead))
                    FAULT ("Failed to read data. Error: " << getErrorMessage (GetLastError()));
            }
            break;
        }
        // 12.
        case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
        {
            auto* bytesAvailable = reinterpret_cast<DWORD*> (statusInformation);
            DBUG ("12. Data available: " << *bytesAvailable << " bytes.");
            if (*bytesAvailable > 0)
            {
                std::vector<char> buffer (*bytesAvailable);
                DWORD             bytesRead = 0;
                if (WinHttpReadData (internet, buffer.data(), *bytesAvailable, &bytesRead))
                    DBUG ("12. Read " << bytesRead << " bytes: " << std::string (buffer.begin(), buffer.begin() + bytesRead) << "\r\n");
                else
                    FAULT ("12! Failed to read data: " << getErrorMessage (GetLastError()) << "\r\n");
            }
            break;
        }
        // 13.
        case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
        {
            // When the read operation is complete, the data will be in lpvStatusInformation.
            DWORD bytesRead = statusInfoLength;
            if (bytesRead > 0)
            {
                responseBuffer.insert (responseBuffer.end(), static_cast<char*> (statusInformation), static_cast<char*> (statusInformation) + bytesRead);
                DBUG ("13. Reading " << bytesRead << " bytes completed.");
            }
            else
            {
                DBUG ("13! No bytes read.");
            }

            DWORD size = 0;
            if (WinHttpQueryDataAvailable (internet, &size) && size > 0)
            {
                std::vector<char> buffer (size);
                DWORD             bytesDownloaded = 0;

                if (WinHttpReadData (internet, buffer.data(), size, &bytesDownloaded))
                    responseBuffer.insert (responseBuffer.end(), buffer.begin(), buffer.begin() + bytesDownloaded);
                else
                    FAULT ("13! Failed to read data. Error: " << getErrorMessage (GetLastError()));
            }
            else
            {
                // End of response.
                requestOwner->onResponseReceived (200, std::string (responseBuffer.begin(), responseBuffer.end()));
                responseBuffer.clear();
            }
            break;
        }
        case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
        {
            // auto* error = reinterpret_cast<WINHTTP_ASYNC_RESULT*> (statusInformation);
            // DBUG ("Request error: " << error->dwError);
            break;
        }


        case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING: DBUG ("14. Closing handle."); break;
        default: DBUG ("Unhandled internetStatus: " << internetStatus); break;
    }
}

class NetworkRequest::Impl
{
public:
    Impl (std::string_view urlToAccess)
      : url (urlToAccess),
        session (WinHttpOpen (NetworkRequest::kUserAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC))
    {
        if (!session)
            throw std::runtime_error ("Failed to open WinHTTP session: " + getErrorMessage (GetLastError()));
    }

    ~Impl() { closeHandles(); }

    void closeHandles()
    {
        if (request)
        {
            // Removing callback...
            if (WinHttpSetStatusCallback (request, nullptr, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, 0) == WINHTTP_INVALID_STATUS_CALLBACK)
                FAULT ("Failed to remove callback: " << getErrorMessage (GetLastError()));

            WinHttpCloseHandle (request);
            request = nullptr;
        }

        if (connect)
        {
            WinHttpCloseHandle (connect);
            connect = nullptr;
        }

        if (session)
        {
            WinHttpCloseHandle (session);
            session = nullptr;
        }

        isFetching.store (false);
    }

    void fetch (std::string_view payload, NetworkRequest* requestOwner, bool async, bool post)
    {
        if (isFetching.load())
            return;

        isFetching.store (true);

        // Preparing the url...
        std::wstring protocol, domain, path;
        NetworkRequest::Impl::splitUrl (url, protocol, domain, path);

        const auto useHttps = protocol == L"https://";

        connect = WinHttpConnect (session, domain.c_str(), useHttps ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, 0);
        if (!connect)
            throw std::runtime_error ("Failed to connect: " + getErrorMessage (GetLastError()));

        // Opening the request...
        request = WinHttpOpenRequest (connect, post ? L"POST" : L"GET",  // HTTP method.
                                      path.c_str(),                      // Request URI.
                                      nullptr,                           // No additional headers (optional).
                                      WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, useHttps ? WINHTTP_FLAG_SECURE : 0);
        if (!request)
            throw std::runtime_error ("Failed to open request: " + getErrorMessage (GetLastError()));

        // Setting callback for asynchronous events...
        if (async)
        {
            if (WinHttpSetStatusCallback (request, httpCallback, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, 0) == WINHTTP_INVALID_STATUS_CALLBACK)
                throw std::runtime_error ("Failed to set callback: " + getErrorMessage (GetLastError()));
        }

        // Creating headers...
        const auto   data    = std::string (payload);
        std::wstring headers = L"Content-Type: application/json\r\nContent-Length: " + std::to_wstring (data.size()) + L"\r\n";

        // Sending the request...
        BOOL results;
        if (!(results = WinHttpSendRequest (request,
                                            headers.c_str(),                                           // Set content type header.
                                            (DWORD) -1L,                                               // Automatically calculate the header size.
                                            0,                                                         // Send the data with the request.
                                            0,                                                         // Length of the data.
                                            0,                                                         // No extra data. or `static_cast<DWORD> (data.size())`
                                            async ? reinterpret_cast<DWORD_PTR> (requestOwner) : 0)))  // Pass requestOwner as context if using asynchronous communication.
            throw std::runtime_error ("Failed to send request: " + getErrorMessage (GetLastError()));


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

        if (requestOwner)
            requestOwner->onResponseReceived (statusCode, response);
    }

    void setStoppedFetching() { isFetching.store (false); }

private:
    HINTERNET         session = nullptr;
    HINTERNET         connect = nullptr;
    HINTERNET         request = nullptr;
    std::string       url     = "";
    std::atomic<bool> isFetching { false };

    static std::wstring stringToWString (const std::string& str)
    {
        // Convert UTF-8 string to wide string (UTF-16) using MultiByteToWideChar
        int          size_needed = MultiByteToWideChar (CP_UTF8, 0, str.c_str(), static_cast<int> (str.size()), nullptr, 0);
        std::wstring wstr (size_needed, 0);
        MultiByteToWideChar (CP_UTF8, 0, str.c_str(), static_cast<int> (str.size()), &wstr[0], size_needed);
        return wstr;
    }

    static void splitUrl (const std::string& fullUrl, std::wstring& protocol, std::wstring& domain, std::wstring& path)
    {
        // Splitting protocol...
        const auto protocol_end = fullUrl.find ("://");

        std::string narrowProtocol = "";
        if (protocol_end != std::string::npos)
            narrowProtocol = std::string_view (fullUrl.data(), protocol_end + 3);
        protocol = NetworkRequest::Impl::stringToWString (narrowProtocol);

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

        domain = NetworkRequest::Impl::stringToWString (narrowDomain);
        path   = NetworkRequest::Impl::stringToWString (narrowPath);
    }
};


NetworkRequest::NetworkRequest (std::string_view urlToAccess) : url (urlToAccess) { }

NetworkRequest::~NetworkRequest()
{
    cancel();
}

void NetworkRequest::onResponseReceived (int statusCode, const std::string& response)
{
    receivedResponse = true;

    pimpl->setStoppedFetching();

    std::cout << "Response received: " << response << std::endl;

    if (statusCode < 300 && callback)
        callback (statusCode, response);

    pimpl.reset();
}

void NetworkRequest::fetch (std::string_view newPayload)
{
    receivedResponse = false;

    pimpl = std::make_unique<Impl> (url);

    payload = newPayload;
    pimpl->fetch (newPayload, this, true, true);
}

}  // namespace foleys

#endif