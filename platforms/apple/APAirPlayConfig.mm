//
//  APAirPlayConfig.m
//  AirDisplay
//
//  Created by Sheen Tian on 2021/7/13.
//

#import "APAirPlayConfig.h"
#import "APAirPlayConfig+internal.h"

@implementation APAirPlayConfig

- (aps::ap_config_ptr)getCObj {
    auto p = std::make_shared<aps::ap_config>();
    p->name(self.name.UTF8String);
    p->macAddress(self.macAddress.UTF8String);
    return p;
}

@end
