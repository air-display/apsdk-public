//
//  APAirPlayConfig.h
//  AirDisplay
//
//  Created by Sheen Tian on 2021/7/13.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface AirPlayConfigAudioFormat : NSObject

@property int type;
@property int audioInputFormats;
@property int audioOutputFormats;

@end

@interface AirPlayConfigAudioLatency : NSObject

@property int type;
@property int inputLatencyMicros;
@property int outputLatencyMicros;
@property(copy) NSString* audioType;

@end

@interface AirPlayConfigDisplay : NSObject

@property int width;
@property int height;
@property float refreshRate;
@property(copy) NSString* uuid;

@end


@interface APAirPlayConfig : NSObject

@property(copy) NSString *name;
@property(copy) NSString *macAddress;
@property(copy) NSString* model;
@property(copy) NSString* sourceVersion;
@property(copy) NSString* pi;
@property(copy) NSString* pk;
@property int vv;
@property int features;
@property int statusFlag;
@property bool publishService;
@property(retain) AirPlayConfigAudioFormat* audioFormat;
@property(retain) AirPlayConfigAudioLatency* audioLatency;
@property(retain) AirPlayConfigDisplay* display;

@end

NS_ASSUME_NONNULL_END
