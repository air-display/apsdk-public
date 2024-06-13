/*
 *  File: APAirPlayCastingDelegate.h
 *  Project: apsdk
 *  Created: Oct 25, 2018
 *  Author: Sheen Tian
 *
 *  This file is part of apsdk (https://github.com/air-display/apsdk-public)
 *  Copyright (C) 2018-2024 Sheen Tian
 *
 *  apsdk is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Foundation,
 *  either version 3 of the License, or (at your option) any later version.
 *
 *  apsdk is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with Foobar.
 *  If not, see <https://www.gnu.org/licenses/>.
 */

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
- (void)onVideoPlay:(uint64_t)sessionId Location:(NSString *)location StartPos:(float)startPos;
- (void)onVideoScrub:(uint64_t)sessionId Position:(float)position;
- (void)onVideoRate:(uint64_t)sessionId Value:(float)value;
- (void)onVideoStop:(uint64_t)sessionId;
- (void)onAcquirePlaybackInfo:(uint64_t)sessionId PlayBackInfo:(id)info;
@end

NS_ASSUME_NONNULL_END
