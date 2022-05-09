//
//  APAirPlayService.m
//  AirDisplay
//
//  Created by Sheen Tian on 2021/7/13.
//

#import "APAirPlayServer.h"
#import "APAirPlayConfig+internal.h"
#import "APAirPlaySession+internal.h"

#include <ap_server.h>

class APSessionHandler : public aps::ap_handler {
private:
    id<APAirPlaySessionDelegate> delegate;
public:
    APSessionHandler(id<APAirPlaySessionDelegate> d) : delegate(d) {}
    
    virtual void on_session_begin(aps::ap_session_ptr session) override {
        @autoreleasepool {
            [delegate onSessionBegin:[APAirPlaySession createFromCObj:session]];
        }
    }

    virtual void on_session_end(const uint64_t session_id) override {
        @autoreleasepool {
            [delegate onSessionEnd:session_id];
        }
    }
};

@implementation APAirPlayServer
{
    aps::ap_server_ptr _server;
    id<APAirPlaySessionDelegate> _sessionDelegate;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _server = std::make_shared<aps::ap_server>();
    }
    return self;
}

- (void)setConfig:(APAirPlayConfig*) config {
    aps::ap_config_ptr p = aps::ap_config::default_instance();
    p->name(config.name.UTF8String);
    p->macAddress(config.macAddress.UTF8String);
    p->publishService(config.publishService);
    _server->set_config(p);
}

- (void)setSessionDelegate:(id<APAirPlaySessionDelegate>) delegate {
    aps::ap_handler_ptr h = std::make_shared<APSessionHandler>(delegate);
    _server->set_handler(h);
}

- (bool)start {
    return _server->start();
}

- (void)stop {
    _server->stop();
}

@end
