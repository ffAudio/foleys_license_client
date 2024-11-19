//
// Created by Daniel Walz on 14.11.24.
//

#if(WIN32)

#include "foleys_NetworkRequest.h"

//#include <wininet.h>


NetworkRequest::NetworkRequest (std::string_view urlToAccess) : url (urlToAccess) { }

NetworkRequest::~NetworkRequest()
{
    cancel();
}

void NetworkRequest::fetch (std::string_view payload)
{

}

void NetworkRequest::cancel()
{}


#endif