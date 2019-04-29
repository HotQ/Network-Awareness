//
//  BV_NLM.m
//  Network-Awareness
//
//  Created by lizhen on 2019/4/29.
//  Copyright © 2019 lizhen. All rights reserved.
//

#import "BVNetworkListManager.h"
#import <netinet/in.h>
#define adapterName "com.github.HotQ"

@implementation BV_NLM

+(SCNetworkReachabilityRef)getZeroAddress 
{
    struct sockaddr_in zeroAddress;
    bzero(&zeroAddress, sizeof(zeroAddress));
    zeroAddress.sin_len = sizeof(zeroAddress);
    zeroAddress.sin_family = AF_INET;
    
    SCNetworkReachabilityRef defaultRouteReachability = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault, (const struct sockaddr *)&zeroAddress);
    return defaultRouteReachability;
}

-(instancetype)initWithRef:(SCNetworkReachabilityRef)reachabilityRef
{
    self = [super init];
    if (self != nil)
    {
        self.dispatchAdapter = dispatch_queue_create(adapterName, NULL);
        self.reachabilityRef = reachabilityRef;
    }
    return self;
}

void callbackEntry( SCNetworkReachabilityRef target, SCNetworkReachabilityFlags flags, void * __nullable info)
{
    [(__bridge BV_NetWorkEvent*)info ConnectivityChanged:flags];
}

-(BOOL)Start
{
    BOOL ret = NO;
    BV_NetWorkEvent *event = [[BV_NetWorkEvent alloc]init];
    SCNetworkReachabilityContext context = {0, (__bridge_retained void *)event, NULL, NULL, NULL};
    if (SCNetworkReachabilitySetCallback(self.reachabilityRef, callbackEntry, &context))
    {
        if (SCNetworkReachabilitySetDispatchQueue(self.reachabilityRef,self.dispatchAdapter))
        {
            ret = YES;
        }
    }
    return ret;
}
-(BOOL)Stop{
    return YES;
}
- (void)dealloc
{
    [self Stop];
    if (self.reachabilityRef != NULL)
    {
        CFRelease(self.reachabilityRef);
    }
}
@end



@implementation BV_NetWorkEvent

-(BOOL)ConnectivityChanged:(SCNetworkReachabilityFlags) flags
{
    if(flags & kSCNetworkReachabilityFlagsReachable)
        NSLog(@"网络重连");
    else{
        NSLog(@"连接丢失");
    }
    return YES;
}

@end
