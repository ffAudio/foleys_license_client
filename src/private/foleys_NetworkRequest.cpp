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

// #include <wininet.h>

namespace foleys
{

NetworkRequest::NetworkRequest (std::string_view urlToAccess) : url (urlToAccess) { }

NetworkRequest::~NetworkRequest()
{
    cancel();
}

void NetworkRequest::fetch (std::string_view payload)
{
    cancel();

    // TODO
}

void NetworkRequest::cancel()
{
    // at least avoid entering the callback
    callback = nullptr;
}


}  // namespace foleys
#endif