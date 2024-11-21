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

void NetworkRequest::fetch (std::string_view payload)
{
    NSMutableURLRequest* urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]];
    [urlRequest setHTTPMethod:@"POST"];

    NSData* requestData = [NSData dataWithBytes:payload.data() length:payload.length()];
    [urlRequest setHTTPBody:requestData];
    NSURLSession* session = [NSURLSession sharedSession];

    auto* task = [session dataTaskWithRequest:urlRequest
                            completionHandler:^(NSData* data, NSURLResponse* response, NSError* error) {
                                if (error)
                                {
                                    if (callback)
                                        callback (500, {});

                                    return;
                                }

                                auto* httpResponse = (NSHTTPURLResponse*) response;

                                if (httpResponse.statusCode < 400 && callback)
                                {
                                    auto        status       = static_cast<int> (httpResponse.statusCode);
                                    NSString*   resultString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
                                    std::string downloaded   = [resultString UTF8String];
                                    callback (status, downloaded);

                                    [resultString release];
                                }
                            }];
    [task resume];

    dataTask = task;
}


}  // namespace foleys
