

#include <sys/time.h>

#import "APAirPlayConfig.h"
#import "APAirPlayConfig+internal.h"

@implementation AirPlayConfigAudioFormat

- (instancetype)init {
    self = [super init];
    if (self) {
        
    }
    return self;
}

@end

@implementation AirPlayConfigAudioLatency

- (instancetype)init {
    self = [super init];
    if (self) {
        
    }
    return self;
}

@end

@implementation AirPlayConfigDisplay

- (instancetype)init {
    self = [super init];
    if (self) {
        
    }
    return self;
}

@end

@implementation APAirPlayConfig {
    NSString* _randomMac;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        self.name = @"aps";
        self.publishService = true;
        self.macAddress = [self generateMacAddress];
        self.model = @"AppleTV3,2";
        self.sourceVersion = @"220.68";
        self.pi = @"b08f5a79-db29-4384-b456-a4784d9e6055";
        self.pk = @"99FD4299889422515FBD27949E4E1E21B2AF50A454499E3D4BE75A4E0F55FE63";
        self.vv = 2;
        self.features = 0x527FFFF7;
        self.statusFlag = 68;
        
        self.display = [[AirPlayConfigDisplay alloc] init];
        self.display.width = 1920;
        self.display.height = 1080;
        self.display.refreshRate = 1.0f / 24;
        self.display.uuid = @"e5f7a68d-7b0f-4305-984b-974f677a150b";
        
        self.audioLatency = [[AirPlayConfigAudioLatency alloc] init];
        self.audioLatency.type = 96;
        self.audioLatency.audioType = @"default";
        self.audioLatency.inputLatencyMicros = 3;
        self.audioLatency.outputLatencyMicros = 79;
        
        self.audioFormat = [[AirPlayConfigAudioFormat alloc] init];
        self.audioFormat.type = 96;
        self.audioFormat.audioInputFormats = 0x01000000;
        self.audioFormat.audioOutputFormats = 0x01000000;
    }
    return self;
}

- (aps::ap_config_ptr)getCObj {
    auto p = std::make_shared<aps::ap_config>();
    p->name(self.name.UTF8String);
    p->macAddress(self.macAddress.UTF8String);
    p->publishService(self.publishService);
    return p;
}

- (NSString*)generateMacAddress {
    if (nil != _randomMac)
      return _randomMac;

    timeval time;
    gettimeofday(&time, NULL);
    long ts = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    _randomMac = [NSString stringWithFormat:@"%02X:%02X:%02X:%02X:%02X:%02X",
        (ts & 0xff), ((ts >> 8) & 0xff),
        ((ts >> 16) & 0xff), ((ts >> 24) & 0xff),
        ((ts >> 32) & 0xff), ((ts >> 40) & 0xff)];
    return _randomMac;
}

@end
