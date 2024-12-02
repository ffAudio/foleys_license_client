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
#include <memory>

namespace foleys
{

class NetworkRequest final
{
public:
    static constexpr auto kUserAgent = L"foleys_license_win/1.0";

    explicit NetworkRequest (std::string_view url);
    ~NetworkRequest();

    void fetch (std::string_view newPayload);

#if (WIN32)

    enum class FetchStateType
    {
        Idle,
        Fetching,
        DoneFetching,
        CancelRequested,
        Closed
    };

    void                             onResponseReceived (int statusCode, const std::string& response);
    void                             onFetchingDone();
    [[nodiscard]] const std::string& getPayload() const noexcept { return payload; }
    [[nodiscard]] bool               hasReceivedResponse() const noexcept { return receivedResponse; }
#endif

    void cancel();

    std::function<void (int, std::string_view)> callback;
    std::string                                 payload = "";

private:
    std::string url;

#if (__APPLE__)
    void* dataTask = nullptr;
#else
    class Impl;
    std::unique_ptr<Impl> pimpl;
    bool                  receivedResponse = false;
#endif

    FOLEYS_DISABLE_COPY (NetworkRequest)
};

}  // namespace foleys


#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_NETWORKREQUEST_H
