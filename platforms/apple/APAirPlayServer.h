//
//  APAirPlayService.h
//  AirDisplay
//
//  Created by Sheen Tian on 2021/7/13.
//

#import <Foundation/Foundation.h>

#import <APSDK/APAirPlayConfig.h>
#import <APSDK/APAirPlaySessionDelegate.h>

NS_ASSUME_NONNULL_BEGIN

@interface APAirPlayServer : NSObject

- (void)setConfig:(APAirPlayConfig*) config;

- (void)setSessionDelegate:(id<APAirPlaySessionDelegate>) delegate;

- (bool)start;

- (void)stop;

@end

NS_ASSUME_NONNULL_END
