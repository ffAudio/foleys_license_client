/********************************************************************************
    Copyright 2024 - Daniel Walz
    Foleys Finest Audio UG (haftungsbeschraenkt)
    Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

    This code is provided under the ISC license

    This code is provided as is. The authors disclaim all warranties.
    For details refer to the LICENSE.md

*******************************************************************************/

#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_NETWORKREQUEST_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_NETWORKREQUEST_H

#include "foleys_Licensing.h"

#include <functional>

namespace foleys
{

class NetworkRequest final
{
public:
    static constexpr auto kUserAgent = L"foleys_license_win/1.0";

    explicit NetworkRequest (std::string_view url);
    ~NetworkRequest();

    void fetch (std::string_view payload, bool async = true, bool post = true);
    void cancel();

    std::function<void (int, std::string_view)> callback;

private:
    std::string url;

    //void* session = nullptr;
   // void* connect = nullptr;
   // void* request = nullptr;

#if (__APPLE__)
    void* dataTask = nullptr;
#endif

    [[nodiscard]] static std::wstring stringToWString (const std::string& str);
    static void                       splitUrl (const std::string& fullUrl, std::wstring& protocol, std::wstring& domain, std::wstring& path);


    FOLEYS_DISABLE_COPY (NetworkRequest)
};

}  // namespace foleys


#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_NETWORKREQUEST_H