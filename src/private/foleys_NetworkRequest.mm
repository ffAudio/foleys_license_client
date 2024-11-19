

#include "foleys_NetworkRequest.h"

#import <Foundation/Foundation.h>

namespace foleys
{

NetworkRequest::NetworkRequest (std::string_view urlToAccess) : url (urlToAccess) { }

NetworkRequest::~NetworkRequest()
{
    cancel();
}

void NetworkRequest::fetch (std::string_view payload)
{
    NSMutableURLRequest* urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]];
    [urlRequest setHTTPMethod:@"POST"];

    NSData* requestData = [NSData dataWithBytes:payload.data() length:payload.length()];
    [urlRequest setHTTPBody:requestData];
    NSURLSession* session = [NSURLSession sharedSession];

    auto* task = [session dataTaskWithRequest:urlRequest
                            completionHandler:^(NSData* data, NSURLResponse* response, NSError* error) {
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

void NetworkRequest::cancel()
{
    if (dataTask)
        [(NSURLSessionDataTask*) dataTask cancel];
}


}  // namespace foleys
