//
//  main.m
//  Network-Awareness
//
//  Created by lizhen on 2019/4/29.
//  Copyright © 2019 lizhen. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "BVNetworkListManager.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        BV_NLM *instance = [[BV_NLM alloc] initWithRef : [BV_NLM getZeroAddress]];
        [instance Start];
        int i = 1;
        while(i<10){
            i++;sleep(1);
            NSLog(@"%d s",i);
        }
    }
    return 0;
}
