//
//  BV_NLM.h
//  Network-Awareness
//
//  Created by lizhen on 2019/4/29.
//  Copyright © 2019 lizhen. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <SystemConfiguration/SystemConfiguration.h>

NS_ASSUME_NONNULL_BEGIN

@interface BV_NetWorkEvent : NSObject

@property (nonatomic, assign) bool m_conAlive;
@property (nonatomic, assign) bool m_inLoop;

-(BOOL)ConnectivityChanged:(SCNetworkReachabilityFlags) reachabilityFlag;

@end


@interface BV_NLM : NSObject

@property (nonatomic, strong) dispatch_queue_t dispatchAdapter;
@property (nonatomic, assign) SCNetworkReachabilityRef reachabilityRef;
@property (nonatomic, retain) BV_NetWorkEvent *netWorkEvent;

+(SCNetworkReachabilityRef)getZeroAddress;

-(instancetype)initWithRef:(SCNetworkReachabilityRef)reachabilityRef;
-(BOOL)Start;
-(BOOL)Stop;
-(void)dealloc;

@end

NS_ASSUME_NONNULL_END
