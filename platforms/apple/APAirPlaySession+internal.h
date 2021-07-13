//
//  APAirPlaySession.h
//  apsdk
//
//  Created by Sheen Tian on 2021/7/13.
//

#import <Foundation/Foundation.h>

#import <APSDK/APAirPlayMirroringDelegate.h>
#import <APSDK/APAirPlayCastingDelegate.h>

#include "../../src/ap_session.h"

NS_ASSUME_NONNULL_BEGIN

@interface APAirPlaySession (internal)

+ (instancetype)createFromCObj:(aps::ap_session_ptr) p;

@end

NS_ASSUME_NONNULL_END
