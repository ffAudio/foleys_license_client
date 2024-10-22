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

#include <string_view>
#include <functional>

namespace foleys
{

class NetworkRequest final
{
public:
    explicit NetworkRequest (std::string_view url);
    ~NetworkRequest();

    void fetch (std::string_view payload);

    void cancel();

    std::function<void (int, std::string_view)> callback;

private:
    std::string url;

#if (__APPLE__)
    void* dataTask = nullptr;
#endif

    FOLEYS_DISABLE_COPY (NetworkRequest)
};

}  // namespace foleys


#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_NETWORKREQUEST_H
