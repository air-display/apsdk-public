//
//  APAirPlaySessionDelegate.h
//  apsdk
//
//  Created by Sheen Tian on 2021/7/13.
//

#import <Foundation/Foundation.h>

#import <APS/APAirPlaySession.h>

NS_ASSUME_NONNULL_BEGIN

@protocol APAirPlaySessionDelegate <NSObject>
@required

- (void)onSessionBegin:(APAirPlaySession*)session;

- (void)onSessionEnd:(uint64_t) sessionId;

@end

NS_ASSUME_NONNULL_END
