/* 
 *  File: APAirPlaySession.mm
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

#import "APAirPlaySession.h"
#import "APAirPlaySession+internal.h"

#include <ap_session.h>

class APMirroringHandler : public aps::ap_mirroring_session_handler {
private:
    id<APAirPlayMirroringDelegate> delegate;
public:
    APMirroringHandler(id<APAirPlayMirroringDelegate> d) : delegate(d) {};
    
    virtual void on_video_stream_started() override {
        @autoreleasepool {
            [delegate onVideoStreamStarted];
        }
    }
    
    virtual void on_video_stream_codec(const aps::sms_video_codec_packet_t *p) override {
        @autoreleasepool {
            NSData* data = [NSData dataWithBytes:p->payload length:p->payload_size];
            [delegate onVideoStreamCodec:data];
        }
    }
    
    virtual void on_video_stream_data(const aps::sms_video_data_packet_t *p) override {
        @autoreleasepool {
            NSData* data = [NSData dataWithBytes:p->payload length:p->payload_size];
            [delegate onVideoStreamData:data Timestamp:p->timestamp];
        }
    }
    
    virtual void on_video_stream_stopped() override {
        @autoreleasepool {
            [delegate onVideoStreamStopped];
        }
    }
    
    virtual void on_video_stream_heartbeat() override {
        @autoreleasepool {
            [delegate onVideoStreamHeartbeat];
        }
    }
    
    virtual void on_audio_set_volume(const float ratio, const float volume) override {
        @autoreleasepool {
            [delegate onAudioSetVolume:ratio Volume:volume];
        }
    }
    
    virtual void on_audio_set_progress(const float ratio, const uint64_t start, const uint64_t current,
                                       const uint64_t end) override {
        @autoreleasepool {
            [delegate onAudioSetProgress:ratio Start:start Current:current End:end];
        }
    }
    
    virtual void on_audio_set_cover(const std::string format, const void *data, const uint32_t length) override {
        @autoreleasepool {
            NSString* f = [NSString stringWithUTF8String:format.c_str()];
            NSData* d = [NSData dataWithBytes:data length:length];
            [delegate onAudioSetCover:f Data:d];
        }
    }
    
    virtual void on_audio_set_meta_data(const void *data, const uint32_t length) override {
        @autoreleasepool {
            NSData* d = [NSData dataWithBytes:data length:length];
            [delegate onAudioSetMetaData:d];
        }
    }
    
    virtual void on_audio_stream_started(const aps::audio_data_format_t format) override {
        // aac-eld 44.1kHz channel = 2
        // |0xF8, 0xE8, 0x50, 0x00
        // |11111 000111 | 0100 | 0010 | 1 0000 0000 0000
        // |  32   +  7  |   4  |   2  |
        uint8_t asc[] = {0xF8, 0xE8, 0x50, 0x00};
        @autoreleasepool {
            [delegate onAudioStreamStarted:(AudioFormat)format];
        }
    }
    
    virtual void on_audio_stream_data(const aps::rtp_audio_data_packet_t *p, const uint32_t payload_length) override {
        @autoreleasepool {
            NSData* d = [NSData dataWithBytes:p->payload length:payload_length];
            [delegate onAudioStreamData:d Timestamp:p->timestamp];
        }
    }
    
    virtual void on_audio_stream_stopped() override {
        @autoreleasepool {
            [delegate onAudioStreamStopped];
        }
    }
};

class APCastingHandler : public aps::ap_casting_session_handler {
private:
    id<APAirPlayCastingDelegate> delegate;
    uint64_t sid;
    
public:
    APCastingHandler(id<APAirPlayCastingDelegate> d, uint64_t session_id) : delegate(d), sid(session_id) {
    };
    
    virtual void on_video_play(const uint64_t session_id, const std::string &location, const float start_pos) override {
        sid = session_id;
        @autoreleasepool {
            NSString* l = [NSString stringWithUTF8String:location.c_str()];
            [delegate onVideoPlay:sid Location:l StartPos:start_pos];
        }
    }
    
    virtual void on_video_scrub(const uint64_t session_id, const float position) override {
        if (sid != session_id) {
            @autoreleasepool {
                [delegate onVideoScrub:session_id Position:position];
            }
        }
    }
    
    virtual void on_video_rate(const uint64_t session_id, const float value) override {
        if (sid != session_id) {
            @autoreleasepool {
                [delegate onVideoRate:session_id Value:value];
            }
        }
    }
    
    virtual void on_video_stop(const uint64_t session_id) override {
        if (sid != session_id) {
            @autoreleasepool {
                [delegate onVideoStop:session_id];
            }
        }
    }
    
    virtual void on_acquire_playback_info(const uint64_t session_id, aps::playback_info_t &playback_info) override {
        if (sid != session_id) {
            @autoreleasepool {
                APPlayBackInfo* playbackInfo = [[APPlayBackInfo alloc] init];
                [delegate onAcquirePlaybackInfo:session_id PlayBackInfo:playbackInfo];
                playback_info.stallCount = (uint32_t) playbackInfo.stallCount;
                playback_info.duration = playbackInfo.duration;
                playback_info.position = playbackInfo.position;
                playback_info.rate = playbackInfo.rate;
                playback_info.readyToPlay = playbackInfo.readyToPlay;
                playback_info.playbackBufferEmpty = playbackInfo.playbackBufferEmpty;
                playback_info.playbackBufferFull = playbackInfo.playbackBufferFull;
                playback_info.playbackLikelyToKeepUp = playbackInfo.playbackLikelyToKeepUp;
            }
        }
    }
};


@implementation APAirPlaySession {
    aps::ap_session_ptr _session;
}

+ (instancetype)createFromCObj:(aps::ap_session_ptr) p {
    APAirPlaySession *s = [[self alloc] init];
    s->_session = p;
    return s;
}

- (uint64_t)getSessionId {
    return _session->get_session_id();
}

- (uint32_t)getSessionType {
    return _session->get_session_type();
}

- (void)disconnect {
    _session->disconnect();
}

- (void)setMirroringDelegate:(id <APAirPlayMirroringDelegate>) delegate {
    aps::ap_mirroring_session_handler_ptr h = std::make_shared<APMirroringHandler>(delegate);
    _session->set_mirroring_handler(h);
}

- (void)setCastingDelegate:(id <APAirPlayCastingDelegate>) delegate {
    aps::ap_casting_session_handler_ptr h = std::make_shared<APCastingHandler>(delegate, _session->get_session_id());
    _session->set_casting_handler(h);
}

@end
