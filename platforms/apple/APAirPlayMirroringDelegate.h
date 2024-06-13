/*
 *  File: APAirPlayMirroringDelegate.h
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

typedef enum AudioFormat { kPCM = 0, kALAC = 1, kAAC = 2, kAACELD = 3 } AudioFormat;

@protocol APAirPlayMirroringDelegate <NSObject>
@required
- (void)onVideoStreamStarted;
- (void)onVideoStreamCodec:(NSData *)config;
- (void)onVideoStreamData:(NSData *)data Timestamp:(uint64_t)timestamp;
- (void)onVideoStreamStopped;
- (void)onVideoStreamHeartbeat;

- (void)onAudioSetVolume:(float)ratio Volume:(float)volume;
- (void)onAudioSetProgress:(float)ratio Start:(uint64_t)start Current:(uint64_t)current End:(uint64_t)end;
- (void)onAudioSetCover:(NSString *)format Data:(NSData *)data;
- (void)onAudioSetMetaData:(NSData *)data;
- (void)onAudioStreamStarted:(AudioFormat)format;
- (void)onAudioStreamData:(NSData *)data Timestamp:(uint64_t)timestamp;
- (void)onAudioStreamStopped;
@end

NS_ASSUME_NONNULL_END
