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
        int i;

        BV_NLM *instance = [[BV_NLM alloc] initWithRef : [BV_NLM getZeroAddress]];
        [instance Start];
        i = 1;while(i<1){ NSLog(@"%d s",i); i++;sleep(1);}
        [instance Stop];

    }
    return 0;
}
