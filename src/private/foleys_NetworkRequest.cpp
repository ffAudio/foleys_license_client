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

void NetworkRequest::fetch (std::string_view payload)
{

}

}  // namespace foleys
#endif