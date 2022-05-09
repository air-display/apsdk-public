//
//  APAirPlayConfig.h
//  AirDisplay
//
//  Created by Sheen Tian on 2021/7/13.
//

#import <Foundation/Foundation.h>

#include <ap_config.h>

NS_ASSUME_NONNULL_BEGIN

@interface APAirPlayConfig (internal)

- (aps::ap_config_ptr)getCObj;

@end

NS_ASSUME_NONNULL_END
