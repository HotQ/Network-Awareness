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
#define NLM_LOOP_WAIT_PERIOD 3
#define TIMEOUT_CHECK_INTERVAL 1

@implementation BV_NetWorkEvent

-(BOOL)ConnectivityChanged:(SCNetworkReachabilityFlags) flags
{
    if(flags & kSCNetworkReachabilityFlagsReachable)
    {
        NSLog(@"重连！");
        if(self.timer != nil)
            [self Pause];
    }
    else
    {
        NSLog(@"Connection Interrupted!!!");
        [self StartWaiting];
        
    }
    return YES;
}

- (void)dealloc
{
    NSLog(@"event awsl");
}

-(void)StartWaiting
{
    if(self.thread == nil){
        NSLog(@"thread created");
        self.thread = [[NSThread alloc] initWithTarget:self selector:@selector(LostWait:) object:nil];
        [self.thread start];
    }else{
        NSLog(@"timer reFired");
        [self.timer setFireDate:[[NSDate date] dateByAddingTimeInterval:NLM_LOOP_WAIT_PERIOD]];
    }
}

-(void)LostWait:(NSTimer *)tempTimer
{
    self.timer = [NSTimer scheduledTimerWithTimeInterval:NLM_LOOP_WAIT_PERIOD repeats:YES block:^(NSTimer * _Nonnull timer) {
        NSLog(@"Connection Lost!!!");
    }];
    NSRunLoop *runLoop = [NSRunLoop currentRunLoop];
    while(![[NSThread currentThread] isCancelled]){
        [runLoop runUntilDate:[NSDate dateWithTimeIntervalSinceNow:TIMEOUT_CHECK_INTERVAL]];
    }
}

-(void)Pause
{
    NSLog(@"timer pause");
    [self.timer setFireDate:[NSDate distantFuture]];
}

-(void)Stop
{
    NSLog(@"BV_NetWorkEvent Stop is called");
    [self.timer invalidate];
    [self.thread cancel];
}

@end



@implementation BV_NLM

+(SCNetworkReachabilityRef)getZeroAddress
{
    /*
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
        // 高版本osx开启ARC已不用dispatch_release()释放。
        self.dispatchAdapter = dispatch_queue_create(adapterName, DISPATCH_QUEUE_SERIAL);
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

-(void)Stop{
    SCNetworkReachabilitySetDispatchQueue(self.reachabilityRef, NULL);
}

- (void)dealloc
{
    [self Stop];
    [self.netWorkEvent Stop];
    if (self.reachabilityRef != NULL)
    {
        CFRelease(self.reachabilityRef);
    }
}

@end
