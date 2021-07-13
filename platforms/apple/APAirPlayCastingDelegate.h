//
//  APAirPlayCastingDelegate.h
//  apsdk
//
//  Created by Sheen Tian on 2021/7/13.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface APPlayBackInfo : NSObject

@property uint32_t stallCount;
@property double duration;
@property float position;
@property double rate;
@property bool readyToPlay;
@property bool playbackBufferEmpty;
@property bool playbackBufferFull;
@property bool playbackLikelyToKeepUp;

@end

@protocol APAirPlayCastingDelegate <NSObject>
@required
- (void)onVideoPlay:(uint64_t) sessionId Location:(NSString*) location StartPos:(float) startPos;
- (void)onVideoScrub:(uint64_t) sessionId Position:(float) position;
- (void)onVideoRate:(uint64_t) sessionId Value:(float) value;
- (void)onVideoStop:(uint64_t) sessionId;
- (void)onAcquirePlaybackInfo:(uint64_t) sessionId PlayBackInfo:(id) info;
@end

NS_ASSUME_NONNULL_END
