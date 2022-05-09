//
//  APAirPlaySession.h
//  apsdk
//
//  Created by Sheen Tian on 2021/7/13.
//

#import <Foundation/Foundation.h>

#import <APS/APAirPlayMirroringDelegate.h>
#import <APS/APAirPlayCastingDelegate.h>

NS_ASSUME_NONNULL_BEGIN

@interface APAirPlaySession : NSObject

- (uint64_t)getSessionId;

- (uint32_t)getSessionType;

- (void)disconnect;

- (void)setMirroringDelegate:(id<APAirPlayMirroringDelegate>) delegate;

- (void)setCastingDelegate:(id<APAirPlayCastingDelegate>) delegate;

@end

NS_ASSUME_NONNULL_END
