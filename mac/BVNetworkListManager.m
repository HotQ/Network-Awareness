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
{    /*
      See Apple's Reachability implementation and read me:
      The address 0.0.0.0, which reachability treats as a special token that
      causes it to actually monitor the general routing status of the device,
      both IPv4 and IPv6.
      https://developer.apple.com/library/ios/samplecode/Reachability/Listings/ReadMe_md.html#//apple_ref/doc/uid/DTS40007324-ReadMe_md-DontLinkElementID_11
      */
    struct sockaddr_in6 zeroAddress;
    bzero(&zeroAddress, sizeof(zeroAddress));
    zeroAddress.sin6_len = sizeof(zeroAddress);
    zeroAddress.sin6_family = AF_INET6;
    
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
    self.netWorkEvent = [[BV_NetWorkEvent alloc]init];
    SCNetworkReachabilityContext context = {0, (__bridge void *)self.netWorkEvent, NULL, NULL, NULL};
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
- (void)dealloc
{
    NSLog(@"awsl....");
}
@end
