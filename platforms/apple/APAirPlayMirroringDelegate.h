//
//  APAirPlayMirroringDelegate.h
//  apsdk
//
//  Created by Sheen Tian on 2021/7/13.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef enum AudioFormat {
    kPCM = 0,
    kALAC = 1,
    kAAC = 2,
    kAACELD = 3
} AudioFormat;

@protocol APAirPlayMirroringDelegate <NSObject>
@required
- (void)onVideoStreamStarted;
- (void)onVideoStreamCodec:(NSData*) config;
- (void)onVideoStreamData:(NSData*) data Timestamp:(uint64_t) timestamp;
- (void)onVideoStreamStopped;
- (void)onVideoStreamHeartbeat;

- (void)onAudioSetVolume:(float) ratio Volume:(float) volume;
- (void)onAudioSetProgress:(float) ratio Start:(uint64_t) start Current:(uint64_t) current End:(uint64_t) end;
- (void)onAudioSetCover:(NSString*) format Data:(NSData*)data;
- (void)onAudioSetMetaData:(NSData*) data;
- (void)onAudioStreamStarted:(AudioFormat) format;
- (void)onAudioStreamData:(NSData*) data Timestamp:(uint64_t)timestamp;
- (void)onAudioStreamStopped;
@end

NS_ASSUME_NONNULL_END
