/********************************************************************************
    Copyright 2024 - Daniel Walz
    Foleys Finest Audio UG (haftungsbeschraenkt)
    Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

    This code is provided under the ISC license

    This code is provided as is. The authors disclaim all warranties.
    For details refer to the LICENSE.md

*******************************************************************************/

#include "foleys_NetworkRequest.h"

#import <Foundation/Foundation.h>

namespace foleys
{

NetworkRequest::NetworkRequest (std::string_view urlToAccess) : url (urlToAccess) { }

NetworkRequest::~NetworkRequest()
{
    callback = nullptr;
    cancel();
}

void NetworkRequest::fetch (std::string_view payload)
{
    cancel();

    NSMutableURLRequest* urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]];
    [urlRequest setHTTPMethod:@"POST"];

    NSData* requestData = [NSData dataWithBytes:payload.data() length:payload.length()];
    [urlRequest setHTTPBody:requestData];
    NSURLSession* session = [NSURLSession sharedSession];

    auto* task = [session dataTaskWithRequest:urlRequest
                            completionHandler:^(NSData* data, NSURLResponse* response, NSError* error) {
                                if (error)
                                    return;

                                auto* httpResponse = (NSHTTPURLResponse*) response;

                                if ((httpResponse.statusCode % 100 == 2) && callback)
                                {
                                    auto        status       = static_cast<int> (httpResponse.statusCode);
                                    NSString*   resultString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
                                    std::string downloaded   = [resultString UTF8String];
                                    callback (status, downloaded);

                                    [resultString release];
                                }
                            }];
    [task retain];
    [task resume];

    dataTask = task;
}

void NetworkRequest::cancel()
{
    if (dataTask)
    {
        auto* task = (NSURLSessionDataTask*)dataTask;
        [task cancel];
        [task release];
        dataTask = nullptr;
    }
}


}  // namespace foleys
