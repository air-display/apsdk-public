#include <chrono>
#include <cstring>
#include <ctime>
#include <exception>
#include <stdexcept>

#include <ap_types.h>
#include <service/ap_airplay_service.h>
#include <service/ap_audio_stream_service.h>
#include <service/ap_content_parser.h>
#include <service/ap_event_connection_manager.h>
#include <service/ap_media_data_store.h>
#include <service/ap_mirror_stream_service.h>
#include <utils/logger.h>
#include <utils/plist.h>
#include <utils/utils.h>

using namespace aps::network;
using namespace aps::service::details;

namespace aps {
namespace service {
ap_airplay_connection::ap_airplay_connection(asio::io_context &io_ctx,
                                             ap_config_ptr &config,
                                             ap_handler_ptr &hanlder,
                                             tcp_service_weak_ptr service)
    : xtxp_connection_base(io_ctx), is_reversed_(false),
      session_type_(unknown_session), mirror_session_handler_(0),
      video_session_handler_(0), config_(config), handler_(hanlder),
      service_(service) {
  init_session_id();
  crypto_ = std::make_shared<ap_crypto>();
  timing_sync_service_ = std::make_shared<ap_timing_sync_service>();
  timing_sync_service_->open();
  initialize_request_handlers();
  LOGD() << "ap_airplay_connection (" << std::hex << this
         << ") is being created";
}

ap_airplay_connection::~ap_airplay_connection() {
  if (timing_sync_service_) {
    timing_sync_service_->close();
    timing_sync_service_.reset();
  }

  if (is_reversed()) {
    ap_event_connection_manager::get().remove(apple_session_id_);
  }

  if (video_session == session_type_) {
    video_session_handler_.reset();
    if (handler_) {
      handler_->on_session_end(session_id_);
    }
  } else if (mirror_session == session_type_) {
    mirror_session_handler_.reset();
    if (handler_) {
      handler_->on_session_end(session_id_);
    }
  }

  LOGD() << "ap_airplay_connection (" << std::hex << this
         << ") is being destroyed";
}

uint64_t ap_airplay_connection::get_session_id() { return session_id_; }

uint32_t ap_airplay_connection::get_session_type() { return session_type_; }

void ap_airplay_connection::set_mirror_handler(
    ap_mirror_session_handler_ptr handler) {
  if (mirror_session == session_type_) {
    mirror_session_handler_ = handler;
  }
}

void ap_airplay_connection::set_video_handler(
    ap_video_session_handler_ptr handler) {
  if (video_session == session_type_) {
    video_session_handler_ = handler;
  }
}

void ap_airplay_connection::disconnect() { stop(); }

void ap_airplay_connection::options_handler(const request &req, response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  res.with_status(ok)
      .with_header("Public", "ANNOUNCE, SETUP, RECORD, PAUSE, FLUSH, TEARDOWN, "
                             "OPTIONS, GET_PARAMETER, SET_PARAMETER, POST, GET")
      .with_content_type(APPLICATION_OCTET_STREAM);
}

void ap_airplay_connection::post_pair_setup_handler(const request &req,
                                                    response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  if (32 == req.content.size()) {
    crypto_->init_client_public_keys(0, 0, req.content.data(), 32);
  }

  res.with_status(ok)
      .with_content(crypto_->server_keys().ed_public_key())
      .with_content_type(APPLICATION_OCTET_STREAM);
}

void ap_airplay_connection::post_pair_verify_handler(const request &req,
                                                     response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  pair_verify_header_t *header = (pair_verify_header_t *)req.content.data();
  if (header->is_first_frame) {
    crypto_->init_client_public_keys(
        req.content.data() + 4, 32,       // client curve public key
        req.content.data() + 4 + 32, 32); // client ed public key

    crypto_->init_pair_verify_aes();

    std::vector<uint8_t> signature;
    crypto_->sign_pair_signature(signature);

    std::vector<uint8_t> res_data = crypto_->server_keys().curve_public_key();
    std::copy(signature.begin(), signature.end(), std::back_inserter(res_data));

    res.with_status(ok).with_content(res_data).with_content_type(
        APPLICATION_OCTET_STREAM);

    return;
  } else {
    if (crypto_->verify_pair_signature(req.content.data() + 4, 64)) {
      res.with_status(ok);
    } else {
      res.with_status(bad_request);
    }
  }
}

void ap_airplay_connection::post_fp_setup_handler(const request &req,
                                                  response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  fp_header_t *header = (fp_header_t *)req.content.data();
  if (header->signature[0] == 'F' && header->signature[1] == 'P' &&
      header->signature[2] == 'L' && header->signature[3] == 'Y') {
    if (header->major_version == 0x03) {
      if (header->phase == 0x01 && req.content.size() == 16) {
        uint8_t mode = req.content[14];
        std::vector<uint8_t> content(142, 0);
        crypto_->fp_setup(mode, content.data());
        res.with_status(ok)
            .with_content_type(APPLICATION_OCTET_STREAM)
            .with_content(content);
      } else if (header->phase == 0x02) {
        LOGE() << "Unsupported FP phase.";
      } else if (header->phase == 0x03 && req.content.size() == 164) {
        // Process the hand shake request
        std::vector<uint8_t> content(32, 0);
        crypto_->fp_handshake(content.data(), req.content.data(),
                              req.content.size());
        res.with_status(ok)
            .with_content_type(APPLICATION_OCTET_STREAM)
            .with_content(content);
      } else {
        LOGE() << "Invalid request";
        res.with_status(bad_request);
        return;
      }
    }
  }
  res.with_status(ok).with_content_type(APPLICATION_OCTET_STREAM);
}

void ap_airplay_connection::setup_handler(const request &req, response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  do {
    if (req.content_type.compare(APPLICATION_BINARY_PLIST))
      break;

    auto_plist plist_obj =
        plist_object_from_bplist(req.content.data(), req.content.size());
    auto data_obj = plist_obj.get();
    if (!data_obj)
      break;

    auto streams = plist_object_dict_get_value(data_obj, "streams");
    if (streams) {
      if (PLIST_TYPE_ARRAY != plist_object_get_type(streams))
        break;

      auto stream_obj = plist_object_array_get_value(streams, 0);
      if (!stream_obj || PLIST_TYPE_DICT != plist_object_get_type(stream_obj))
        break;

      auto type_obj = plist_object_dict_get_value(stream_obj, "type");
      if (!type_obj || PLIST_TYPE_INTEGER != plist_object_get_type(type_obj))
        break;

      int64_t type = 0;
      if (0 != plist_object_integer_get_value(type_obj, &type))
        break;

      if (stream_type_t::audio == type) {
        //{
        //  streams = ({
        //    audioFormat = 262144;
        //    audioMode = default;
        //    controlPort = 54956;
        //    ct = 2;
        //    latencyMax = 88200;
        //    latencyMin = 11025;
        //    spf = 352;
        //    type = 96;
        //  });
        //}
        crypto_->init_audio_stream_aes_cbc();

        auto format_obj =
            plist_object_dict_get_value(stream_obj, "audioFormat");
        if (!format_obj ||
            PLIST_TYPE_INTEGER != plist_object_get_type(format_obj))
          break;

        int64_t format_value = 0;
        if (0 != plist_object_integer_get_value(format_obj, &format_value))
          break;

        int format = 0;
        if (format_value == alac)
          format = 1;
        else if (format_value == aac_main)
          format = 2;
        else if (format_value == aac_eld)
          format = 3;
        else {
          LOGE() << "Unsupported audio format " << format_value;
          break;
        }

        if (!audio_stream_service_) {
          audio_stream_service_ = std::make_shared<ap_audio_stream_service>(
              crypto_, mirror_session_handler_);
          audio_stream_service_->start();
        }

        if (mirror_session_handler_) {
          mirror_session_handler_->on_audio_stream_started(
              (audio_data_format_t)format);
        }

        // clang-format off
        auto_plist audio_stream = plist_object_dict(1, 
            "streams", plist_object_array(1,
                plist_object_dict(3, 
                    "type", plist_object_integer(type), 
                    "dataPort", plist_object_integer(audio_stream_service_->data_port()),
                    "controlPort",plist_object_integer(audio_stream_service_->control_port())
                )
            )
        );
        // clang format-on

        res.with_status(ok)
            .with_content(audio_stream.to_bytes_array())
            .with_content_type(APPLICATION_BINARY_PLIST);

        return;
      } else if (stream_type_t::video == type) {
        //{ 
        //  streams = [{
        //    type = 110;
        //    streamConnectionID = 3873339193950750702;
        //    timestampInfo = [
        //      { name = SubSu; },
        //      { name = BePxT; }, 
        //      { name = AfPxT; },
        //      { name = BefEn; },
        //      { name = EmEnc; }
        //    ];
        //  }];
        //}
        auto connection_id_obj =
            plist_object_dict_get_value(stream_obj, "streamConnectionID");
        if (PLIST_TYPE_INTEGER != plist_object_get_type(connection_id_obj))
          break;

        int64_t connection_id = 0;
        if (0 !=
            plist_object_integer_get_value(connection_id_obj, &connection_id))
          break;

        crypto_->init_video_stream_aes_ctr(connection_id, agent_version_);

        if (mirror_session_handler_) {
          mirror_session_handler_->on_mirror_stream_started();
        }

        if (!mirror_stream_service_) {
          mirror_stream_service_ =
              std::make_shared<ap_mirror_stream_service>(crypto_, 0, mirror_session_handler_);
          mirror_stream_service_->start();
        }

        uint16_t listen_port = 0;
        auto service = service_.lock();
        if (service) {
          listen_port = service->port();
        }

        // clang-format off
        auto_plist video_stream = plist_object_dict(2, 
            "eventPort", plist_object_integer(listen_port),
            "streams", plist_object_array(1, 
                plist_object_dict(2, 
                    "type", plist_object_integer(type), 
                    "dataPort", plist_object_integer(mirror_stream_service_->port())
                )
            )
        );
        // clang-format on

        res.with_status(ok)
            .with_content(video_stream.to_bytes_array())
            .with_content_type(APPLICATION_BINARY_PLIST);

        return;
      } else
        break;
    } else {
      //{
      //  deviceID = "4C:57:CA:46:07:FC";
      //  diagnosticsAndUsage = 1;
      //  eiv = <5e830184 fea012b8 3bbd456d b761f798>;
      //  ekey = <data>;
      //  et = 32;
      //  internalBuild = 1;
      //  isScreenMirroringSession = 1;
      //  macAddress = "4C:57:CA:46:07:FA";
      //  model = "iPhone8,4";
      //  name = "hr\U7684 iPhone";
      //  osBuildVersion = 16B92;
      //  osName = "iPhone OS";
      //  osVersion = "12.1";
      //  sessionUUID = "42CD0A19-2E4B-4355-BED6-575FA83A60DA";
      //  sourceVersion = "373.9.1";
      //  timingPort = 63254;
      //  timingProtocol = NTP;
      //}
      const uint8_t *piv = 0;
      uint64_t iv_len = 0;
      auto eiv_obj = plist_object_dict_get_value(data_obj, "eiv");
      if (0 != plist_object_data_get_value(eiv_obj, &piv, &iv_len))
        break;

      const uint8_t *pkey = 0;
      uint64_t key_len = 0;
      auto ekey_obj = plist_object_dict_get_value(data_obj, "ekey");
      if (0 != plist_object_data_get_value(ekey_obj, &pkey, &key_len))
        break;

      crypto_->init_client_aes_info(piv, iv_len, pkey, key_len);

      auto timing_port_obj =
          plist_object_dict_get_value(data_obj, "timingPort");
      if (PLIST_TYPE_INTEGER != plist_object_get_type(timing_port_obj))
        break;
      int64_t timing_port = 0;
      if (0 != plist_object_integer_get_value(timing_port_obj, &timing_port))
        break;

      session_type_ = mirror_session;
      if (handler_) {
        handler_->on_session_begin(shared_from_this());
      }

      timing_sync_service_->set_server_endpoint(
          socket_.remote_endpoint().address(), (uint16_t)timing_port);

      auto_plist content = plist_object_dict(
          2, "eventPort", plist_object_integer(0), "timingPort",
          plist_object_integer(timing_sync_service_->port()));

      res.with_status(ok)
          .with_content_type(APPLICATION_BINARY_PLIST)
          .with_content(content.to_bytes_array());

      return;
    }
  } while (0);

  // Bad request
  res.with_status(bad_request);
  return;
}

void ap_airplay_connection::get_info_handler(const request &req,
                                             response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  // clang-format off
  auto_plist info = plist_object_dict(15,
      "deviceID", plist_object_string(config_->deviceID().c_str()),
      "features", plist_object_integer(config_->features()),
      "keepAliveLowPower", plist_object_integer(1), 
      "keepAliveSendStatsAsBody", plist_object_integer(1), 
      "macAddress", plist_object_string(config_->macAddress().c_str()), 
      "model", plist_object_string(config_->model().c_str()), 
      "name", plist_object_string(config_->name().c_str()), 
      "sourceVersion", plist_object_string(config_->serverVersion().c_str()), 
      "statusFlags", plist_object_integer(config_->statusFlag()), 
      "pi", plist_object_string(config_->pi().c_str()), 
      "pk", plist_object_data((uint8_t *)config_->pk().c_str(), config_->pk().length()),
      "vv", plist_object_integer(config_->vv()), 
      "audioFormats",
      plist_object_array(1,
          plist_object_dict(3, 
              "type", plist_object_integer(96), 
              "audioInputFormats", plist_object_integer(0x01000000),
              "audioOutputFormats", plist_object_integer(0x01000000))),
      "audioLatencies", plist_object_array(1,
          plist_object_dict(4, 
              "type", plist_object_integer(96), 
              "audioType", plist_object_string("default"),
              "inputLatencyMicros", plist_object_integer(0),
              "outputLatencyMicros", plist_object_integer(0)
          )
      ),
      "displays", plist_object_array(1,
          plist_object_dict(11,
              "features", plist_object_integer(14),
              "height", plist_object_integer(config_->display().height()), 
              "heightPixels", plist_object_integer(config_->display().height()),
              "heightPhysical", plist_object_integer(0), 
              "width", plist_object_integer(config_->display().width()), 
              "widthPixels", plist_object_integer(config_->display().width()), 
              "widthPhysical", plist_object_integer(0), 
              "refreshRate", plist_object_real(config_->display().refreshRate()),
              "overscanned", plist_object_true(), 
              "rotation", plist_object_true(), 
              "uuid", plist_object_string(config_->display().uuid().c_str())
          )
      )
  );
  // clang-format on

  res.with_status(ok)
      .with_content_type(APPLICATION_BINARY_PLIST)
      .with_content(info.to_bytes_array());
}

void ap_airplay_connection::post_feedback_handler(const request &req,
                                                  response &res) {
  // DUMP_REQUEST_WITH_CONNECTION(req);
  res.with_status(ok);
}

void ap_airplay_connection::record_handler(const request &req, response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  timing_sync_service_->post_send_query();

  res.with_status(ok).with_header("Audio-Latency", "0");
}

void ap_airplay_connection::get_parameter_handler(const request &req,
                                                  response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  res.with_status(ok)
      .with_content_type(TEXT_PARAMETERS)
      .with_content("volume: 0.000000\r\n");
}

void ap_airplay_connection::post_audioMode(const request &req, response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  res.with_status(ok);
}

void ap_airplay_connection::set_parameter_handler(const request &req,
                                                  response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);
  std::string content(req.content.begin(), req.content.end());
  if (0 == req.content_type.compare(TEXT_PARAMETERS)) {
    float ratio = 0;
    float volume = 0;
    uint64_t start = 0;
    uint64_t current = 0;
    uint64_t end = 0;

    // volume: -11.123877
    if (ap_content_parser::get_volume_value(volume, content.c_str())) {
      if (mirror_session_handler_) {
        if (0 == volume)
          ratio = 100;
        else if (-144.0f == volume)
          ratio = 0;
        else {
          ratio = 100.0f * (30 + volume) / 30;
        }

        mirror_session_handler_->on_audio_set_volume(ratio, volume);
      }
    }
    // progress: 1146221540/1146549156/1195701740  start/current/end
    else if (ap_content_parser::get_progress_values(start, current, end,
                                                    content.c_str())) {
      if (mirror_session_handler_) {
        ratio = 100.0f * (current - start) / (end - start);
        mirror_session_handler_->on_audio_set_progress(ratio, start, current,
                                                       end);
      }
    }
  } else if (0 == req.content_type.compare(IMAGE_JPEG) ||
             0 == req.content_type.compare(IMAGE_PNG)) {
    // body is image data
    if (mirror_session_handler_) {
      mirror_session_handler_->on_audio_set_cover(
          req.content_type, req.content.data(), req.content.size());
    }
  } else if (0 == req.content_type.compare(APPLICATION_DMAP_TAGGED)) {
    // body is dmap data
    if (mirror_session_handler_) {
      mirror_session_handler_->on_audio_set_meta_data(req.content.data(),
                                                      req.content.size());
    }
  } else {
    LOGE() << "Unknown parameter type: " << req.content_type;
  }

  res.with_status(ok);
}

void ap_airplay_connection::teardown_handler(const request &req,
                                             response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  if (0 == req.content_type.compare(APPLICATION_BINARY_PLIST)) {
    auto_plist data_obj =
        plist_object_from_bplist(req.content.data(), req.content.size());
    if (data_obj) {
      auto streams = plist_object_dict_get_value(data_obj, "streams");
      if (streams && PLIST_TYPE_ARRAY == plist_object_get_type(streams)) {
        auto stream_obj = plist_object_array_get_value(streams, 0);
        if (stream_obj &&
            PLIST_TYPE_DICT == plist_object_get_type(stream_obj)) {
          auto type_obj = plist_object_dict_get_value(stream_obj, "type");
          if (type_obj &&
              PLIST_TYPE_INTEGER == plist_object_get_type(type_obj)) {
            int64_t type = 0;
            plist_object_integer_get_value(type_obj, &type);
            if (stream_type_t::video == type) {
              // Stop video stream
              if (mirror_stream_service_) {
                mirror_stream_service_->stop();
                mirror_stream_service_.reset();
              }

              if (mirror_session_handler_) {
                mirror_session_handler_->on_mirror_stream_stopped();
              }

              LOGD() << "Mirroring video stream disconnected";
            } else if (stream_type_t::audio == type) {
              // Stop audio stream
              if (audio_stream_service_) {
                audio_stream_service_->stop();
                audio_stream_service_.reset();
              }

              if (mirror_session_handler_) {
                mirror_session_handler_->on_audio_stream_stopped();
              }

              LOGD() << "Audio stream disconnected";
            } else
              LOGE() << "Unknown stream type";
          }
        }
      }
    }
  }

  res.with_status(ok);
}

void ap_airplay_connection::flush_handler(const request &req, response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  auto rtp_info_header = req.headers.find("RTP-Info");
  if (rtp_info_header != req.headers.end()) {
    auto rtp_info = rtp_info_header->second;
    if (!strncmp(rtp_info.c_str(), "seq=", 4)) {
      uint64_t next_seq = strtol(rtp_info.c_str() + 4, 0, 10);
      // Flush
    }
  }

  res.with_status(ok).with_content_type(APPLICATION_BINARY_PLIST);
}

void ap_airplay_connection::get_server_info_handler(const request &req,
                                                    response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);
  res.with_status(ok);
  // return;

  std::ostringstream oss;
  // clang-format off
  oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
         "<plist version=\"1.0\">\n"
         "<dict>\n"
         "\t<key>features</key>\n"
         "\t<integer>" << /*config_->features()*/"0x27F" << "</integer>\n"
         "\t<key>macAddress</key>\n"
         "\t<string>" << config_->macAddress() << "</string>\n"
         "\t<key>model</key>\n"
         "\t<string>" << config_->model() /*"AppleTV3,2"*/ << "</string>\n"
         "\t<key>osBuildVersion</key>\n"
         "\t<string>12B435</string>\n"
         "\t<key>protovers</key>\n"
         "\t<string>1.0</string>\n"
         "\t<key>srcvers</key>\n"
         "\t<string>" << config_->serverVersion()/*"101.28"*/ << "</string>\n"
         "\t<key>vv</key>\n"
         "\t<integer>" << config_->vv() << "</integer>\n"
         "\t<key>deviceid</key>\n"
         "\t<string>" << config_->macAddress() << "</string>\n"
         "</dict>\n"
         "</plist>";
  // clang-format on

  session_type_ = video_session;
  if (handler_) {
    handler_->on_session_begin(shared_from_this());
  }

  res.with_status(ok)
      .with_content_type(TEXT_APPLE_PLIST_XML)
      .with_content(oss.str());
}

void ap_airplay_connection::post_fp_setup2_handler(const request &req,
                                                   response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  res.with_status(ok);
}

void ap_airplay_connection::post_reverse_handler(const request &req,
                                                 response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  auto purpose = req.headers.find("X-Apple-Purpose");
  if (purpose != req.headers.end()) {
    LOGD() << "Reverse purpose: " << purpose->second;
  }

  auto connection = req.headers.find("Connection");
  if (connection != req.headers.end()) {
    res.with_header(connection->first, connection->second);
  }

  auto upgrade = req.headers.find("Upgrade");
  if (upgrade != req.headers.end()) {
    res.with_header(upgrade->first, upgrade->second);
  }

  auto it_session_id = req.headers.find("X-Apple-Session-ID");
  if (it_session_id != req.headers.end()) {
    LOGD() << "Session_id: " << it_session_id->second;
  }

  reverse_connection(it_session_id->second);

  res.with_status(switching_protocols);
}

void ap_airplay_connection::post_play_handler(const request &req,
                                              response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  std::string location;

  if (0 == compare_string_no_case(req.content_type.c_str(),
                                  APPLICATION_BINARY_PLIST)) {
    // The request content is binary plist:
    //{
    //    "Content-Location" = "mlhls://localhost/master.m3u8";
    //    "Start-Position" = "0.05959678";
    //    "Start-Position-Seconds" = "57.074648";
    //    SenderMACAddress = "4C:57:CA:46:07:FA";
    //    authMs = 0;
    //    bonjourMs = 0;
    //    clientBundleID = "com.google.ios.youtube";
    //    clientProcName = YouTube;
    //    connectMs = 229;
    //    infoMs = 193;
    //    mightSupportStorePastisKeyRequests = 1;
    //    model = "iPhone8,4";
    //    osBuildVersion = 16B92;
    //    playbackRestrictions = 0;
    //    postAuthMs = 0;
    //    rate = 1;
    //    referenceRestrictions = 0;
    //    secureConnectionMs = 0;
    //    streamType = 1;
    //    uuid = "CA558786-81DC-4F5F-BE5A-30869D597576";
    //    volume = 1;
    //}
    auto_plist data_obj =
        plist_object_from_bplist(req.content.data(), req.content.size());
    if (!data_obj) {
      res.with_status(bad_request);
      return;
    }

#if !defined(NDEBUG)
    std::string obj_str = data_obj.dump();
    LOGD() << obj_str;
#endif // !

    const char *id = 0;
    auto uuid_obj = plist_object_dict_get_value(data_obj, "uuid");
    if (0 != plist_object_string_get_value(uuid_obj, &id)) {
      res.with_status(bad_request);
      return;
    }
    playback_uuid_ = id;

    const char *l = 0;
    auto content_location_obj =
        plist_object_dict_get_value(data_obj, "Content-Location");
    if (0 != plist_object_string_get_value(content_location_obj, &l)) {
      res.with_status(bad_request);
      return;
    }
    location = l;

    double pos_in_second = 0;
    auto start_position_obj =
        plist_object_dict_get_value(data_obj, "Start-Position-Seconds");
    if (0 != plist_object_real_get_value(start_position_obj, &pos_in_second)) {
      LOGI() << "No Start-Position-Seconds in request";
    }

    // Adjust this value to milliseconds
    start_pos_in_ms_ = (float)pos_in_second * 1000.f;
  } else if (0 == compare_string_no_case(req.content_type.c_str(),
                                         TEXT_PARAMETERS) ||
             0 == compare_string_no_case(req.content_type.c_str(),
                                         APPLICATION_OCTET_STREAM)) {
    // The request content is text:
    // req.body:
    //      Content-Location:URL
    //      Start-Position: float
    std::string content(req.content.begin(), req.content.end());
    if (!ap_content_parser::get_play_parameters(location, start_pos_in_ms_,
                                                content.c_str())) {
      LOGW() << "No start position found, set to 0";
    }
  } else {
    LOGW() << "Unknown content type :" << req.content_type
           << " for uri: " << req.uri;
    res.with_status(bad_request);
    return;
  }

  auto it_session_id = req.headers.find("X-Apple-Session-ID");
  if (it_session_id == req.headers.end()) {
    res.with_status(bad_request);
    return;
  }

  if (!ap_media_data_store::get().request_media_data(
          location.c_str(), it_session_id->second.c_str())) {
    // Normal URL
    if (video_session_handler_) {
      video_session_handler_->on_video_play(session_id_, location,
                                            start_pos_in_ms_);
    }
  }

  res.with_status(ok);
}

void ap_airplay_connection::post_scrub_handler(const request &req,
                                               response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);
  // /scrub?position=1298.000000
  float pos_in_ms = 0.0f;
  if (ap_content_parser::get_scrub_position(pos_in_ms, req.uri.c_str())) {
    if (video_session_handler_) {
      pos_in_ms = pos_in_ms * 1000;
      video_session_handler_->on_video_scrub(session_id_, pos_in_ms);
    }
  }

  res.with_status(ok);
}

void ap_airplay_connection::post_rate_handler(const request &req,
                                              response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);
  // /rate?value=0.000000
  float value = 0.0f;
  if (ap_content_parser::get_rate_value(value, req.uri.c_str())) {
    if (video_session_handler_) {
      video_session_handler_->on_video_rate(session_id_, value);
    }
  }

  res.with_status(ok);
}

void ap_airplay_connection::post_stop_handler(const request &req,
                                              response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  ap_media_data_store::get().reset();

  if (video_session_handler_) {
    video_session_handler_->on_video_stop(session_id_);
  }

  res.with_status(ok);
}

void ap_airplay_connection::post_action_handler(const request &req,
                                                response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);
  // req.body (bplist)

  auto_plist data_obj =
      plist_object_from_bplist(req.content.data(), req.content.size());
  if (!data_obj) {
    res.with_status(bad_request);
    return;
  }

  const char *type = 0;
  auto type_obj = plist_object_dict_get_value(data_obj, "type");
  if (0 != plist_object_string_get_value(type_obj, &type)) {
    res.with_status(bad_request);
    return;
  }

  if (0 == compare_string_no_case(type, "unhandledURLResponse")) {
    LOGD() << "Action type: " << type;
    //{
    //  type = unhandledURLResponse;
    //  params = {
    //    "FCUP_Response_Data" = <>
    //    "FCUP_Response_RequestID" = 1;
    //    "FCUP_Response_StatusCode" = 0;
    //    "FCUP_Response_URL" = "mlhls://localhost/master.m3u8";
    //  };
    //}
    auto params_dict = plist_object_dict_get_value(data_obj, "params");

    auto fcup_uri_obj =
        plist_object_dict_get_value(params_dict, "FCUP_Response_URL");
    const char *fcup_uri = 0;
    if (0 != plist_object_string_get_value(fcup_uri_obj, &fcup_uri)) {
      res.with_status(bad_request);
      return;
    }

    auto fcup_data_obj =
        plist_object_dict_get_value(params_dict, "FCUP_Response_Data");
    const uint8_t *fcup_data = 0;
    uint64_t data_len = 0;
    if (0 !=
        plist_object_data_get_value(fcup_data_obj, &fcup_data, &data_len)) {
      res.with_status(bad_request);
      return;
    }

    std::string fcup_content;
    fcup_content.assign((char *)fcup_data, (size_t)data_len);

    auto location =
        ap_media_data_store::get().process_media_data(fcup_uri, fcup_content);
    if (!location.empty()) {
      if (video_session_handler_) {
        auto it = req.headers.find(HEADER_APPLE_SESSION_ID);
        if (it != req.headers.end()) {
          apple_session_id_ = it->second;
        }
        video_session_handler_->on_video_play(session_id_, location,
                                              start_pos_in_ms_);
      }
    }

    res.with_content_type(TEXT_APPLE_PLIST_XML)
        .with_content(ERROR_STATUS_RESPONSE);
  } else if (0 == compare_string_no_case(type, "playlistInster")) {
    LOGD() << "Action type: " << type << ". Add new playback.";

  } else if (0 == compare_string_no_case(type, "playlistRemove")) {
    LOGD() << "Action type: " << type << ". Stop playback.";

  } else {
    LOGW() << "Unknown action type: " << type;
  }

  res.with_status(ok);
}

void ap_airplay_connection::get_playback_info_handler(const request &req,
                                                      response &res) {
  // DUMP_REQUEST_WITH_CONNECTION(req);

  playback_info_t playback_info;
  if (video_session_handler_) {
    video_session_handler_->on_acquire_playback_info(session_id_,
                                                     playback_info);
  }
  LOGV() << "PlaybackInfo duration: " << playback_info.duration
         << ", position: " << playback_info.position;
  // Return binary plist
  // clang-format off
  // auto_plist info = plist_object_dict(10,
  //     //"uuid", plist_object_string(""),
  //     "stallCount", plist_object_integer(0),
  //     "duration", plist_object_real(playback_info.duration),
  //     "position", plist_object_real(playback_info.position),
  //     "rate", plist_object_real(playback_info.rate),
  //     "readyToPlay", plist_object_integer(1),
  //     "playbackBufferEmpty", plist_object_integer(1),
  //     "playbackBufferFull", plist_object_integer(0),
  //     "playbackLikelyToKeepUp", plist_object_integer(1),
  //     "loadedTimeRanges", plist_object_array(1, 
  //         plist_object_dict(2,
  //             "start", plist_object_real(0),
  //             "duration", plist_object_real(playback_info.duration)
  //         )
  //     ), 
  //     "seekableTimeRanges", plist_object_array(1, 
  //         plist_object_dict(2,
  //             "start", plist_object_real(0),
  //             "duration", plist_object_real(0)
  //         )
  //     )
  // );
  // clang-format on
  // res.with_status(ok)
  //     .with_content_type(APPLICATION_BINARY_PLIST)
  //     .with_content(info.to_bytes_array());

  std::ostringstream oss;
  // clang-format off
  oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
         "<plist version=\"1.0\">\n"
         "<dict>\n"
         "\t<key>duration</key>\n"
         "\t<real>" << playback_info.duration << "</real>\n"
         "\t<key>position</key>\n"
         "\t<real>" << playback_info.position << "</real>\n"
         "\t<key>rate</key>\n"
         "\t<real>" << playback_info.rate << "</real>\n"
         "\t<key>readyToPlay</key>\n"
         "\t<integer>" << playback_info.readyToPlay << "</integer>\n"
         "\t<key>playbackBufferEmpty</key>\n"
         "\t<integer>1</integer>\n"
         "\t<key>playbackBufferFull</key>\n"
         "\t<integer>0</integer>\n"
         "\t<key>playbackLikelyToKeepUp</key>\n"
         "\t<integer>1</integer>\n"
         "\t<key>loadedTimeRanges</key>\n"
         "\t<array>\n"
         "\t\t<dict>\n"
         "\t\t\t<key>duration</key>\n"
         "\t\t\t<real>" << playback_info.duration << "</real>\n"
         "\t\t\t<key>start</key>\n"
         "\t\t\t<real>0.0</real>\n"
         "\t\t</dict>\n"
         "\t</array>\n"
         "\t<key>seekableTimeRanges</key>\n"
         "\t<array>\n"
         "\t\t<dict>\n"
         "\t\t\t<key>duration</key>\n"
         "\t\t\t<real>" << playback_info.duration << "</real>\n"
         "\t\t\t<key>start</key>\n"
         "\t\t\t<real>0.0</real>\n"
         "\t\t</dict>\n"
         "\t</array>\n"
         "</dict>\n"
         "</plist>";
  // clang-format on

  res.with_status(ok)
      .with_content_type(TEXT_APPLE_PLIST_XML)
      .with_content(oss.str());
}

void ap_airplay_connection::put_setProperty_handler(const request &req,
                                                    response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  if (std::string::npos != req.uri.find("actionAtItemEnd")) {
    // /setProperty?actionAtItemEnd
    //{
    //  value = 1;
    //}
    // advance = 0; play next item
    // pause = 1;   puase the playback
    // none = 2;    do nothing
    do {
      auto_plist data_obj =
          plist_object_from_bplist(req.content.data(), req.content.size());
      if (!data_obj) {
        LOGE() << "Failed to pares the actionAtItemEnd value";
        break;
      }
      auto value_obj = plist_object_dict_get_value(data_obj, "value");
      if (!value_obj) {
        LOGE() << "Failed to pares the actionAtItemEnd value";
        break;
      }
      int64_t value = 0;
      if (0 != plist_object_integer_get_value(value_obj, &value)) {
        LOGE() << "Failed to pares the actionAtItemEnd value";
        break;
      }

      LOGD() << "actionAtItemEnd value " << value;
    } while (0);

    res.with_content_type(TEXT_APPLE_PLIST_XML)
        .with_content(ERROR_STATUS_RESPONSE);
  } else if (std::string::npos != req.uri.find("forwardEndTime")) {
    //{
    //  value {
    //    epoch = 0;
    //    flags = 0;
    //    timescale = 0;
    //    value = 0;
    //  }
    //}
    res.with_content_type(TEXT_APPLE_PLIST_XML)
        .with_content(ERROR_STATUS_RESPONSE);
  } else if (std::string::npos != req.uri.find("reverseEndTime")) {
    // /setProperty?reverseEndTime
    //{
    //  value {
    //    epoch = 0;
    //    flags = 0;
    //    timescale = 0;
    //    value = 0;
    //  }
    //}
    res.with_content_type(TEXT_APPLE_PLIST_XML)
        .with_content(ERROR_STATUS_RESPONSE);
  } else {
    LOGW() << "Unknown property: " << req.uri;
  }

  res.with_status(ok);
}

void ap_airplay_connection::post_getProperty_handler(const request &req,
                                                     response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);
  // GET /getProperty?playbackAccessLog
  // GET /getProperty?playbackErrorLog

  res.with_status(ok);
}

std::shared_ptr<xtxp_connection_base>
ap_airplay_connection::shared_from_self() {
  return shared_from_this();
}

void ap_airplay_connection::add_common_header(const request &req,
                                              response &res) {
  res.with_header(HEADER_DATE, gmt_time_string());

  static std::string ver = "AirTunes/" + config_->serverVersion();
  res.with_header(HEADER_SERVER, ver).with_header(HEADER_SESSION, "CAFEBABE");

  auto it_cseq = req.headers.find(HEADER_CSEQ);
  if (it_cseq != req.headers.end()) {
    res.with_header(HEADER_CSEQ, it_cseq->second);
  }

  if (0 != req.method.compare("RECORD") &&
      0 != req.method.compare("SET_PARAMETER"))
    res.with_header(HEADER_AUDIO_JACK_STATUS, "Connected; type=digital");
}

void ap_airplay_connection::init_session_id() {
  uint64_t address = (uint64_t)this;
  uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();
  session_id_ = (timestamp << 32) | (address & 0x0ffffffff);
}

void ap_airplay_connection::validate_user_agent(const request &req) {
  if (!agent_.empty())
    return;

  auto user_agent_header = req.headers.find("User-Agent");
  if (user_agent_header != req.headers.end()) {
    auto user_agent = user_agent_header->second;
    ap_content_parser::get_user_agent_version(agent_, agent_version_,
                                              user_agent.c_str());

    int major_ver = agent_version_.major;
    if (major_ver >= 370) {
      LOGD() << "Agent Version: " << agent_version_.major << ", iOS12";
    } else if (major_ver >= 350) {
      LOGD() << "Agent Version: " << agent_version_.major << ", iOS11";
    } else if (major_ver >= 300) {
      LOGD() << "Agent Version: " << agent_version_.major << ", iOS10";
    } else if (major_ver >= 230) {
      LOGD() << "Agent Version: " << agent_version_.major << ", iOS9";
    } else {
      LOGD() << "Agent Version: " << agent_version_.major << ", iOS8";
    }
  }
}

#define RH(x)                                                                  \
  std::bind(&ap_airplay_connection::x, this, std::placeholders::_1,            \
            std::placeholders::_2)

void ap_airplay_connection::initialize_request_handlers() {
  // The request route table
  request_route_t routes_table[] = {
      {"RTSP", "OPTIONS", "", RH(options_handler)},
      {"RTSP", "POST", "/pair-setup", RH(post_pair_setup_handler)},
      {"RTSP", "POST", "/pair-verify", RH(post_pair_verify_handler)},
      {"RTSP", "POST", "/fp-setup", RH(post_fp_setup_handler)},
      {"RTSP", "SETUP", "*", RH(setup_handler)},
      {"RTSP", "GET", "/info", RH(get_info_handler)},
      {"RTSP", "POST", "/feedback", RH(post_feedback_handler)},
      {"RTSP", "RECORD", "*", RH(record_handler)},
      {"RTSP", "GET_PARAMETER", "*", RH(get_parameter_handler)},
      {"RTSP", "SET_PARAMETER", "*", RH(set_parameter_handler)},
      {"RTSP", "TEARDOWN", "*", RH(teardown_handler)},
      {"RTSP", "FLUSH", "*", RH(flush_handler)},
      {"RTSP", "POST", "/audioMode", RH(post_audioMode)},
      {"HTTP", "GET", "/server-info", RH(get_server_info_handler)},
      {"HTTP", "POST", "/fp-setup", RH(post_fp_setup_handler)},
      {"HTTP", "POST", "/fp-setup2", RH(post_fp_setup2_handler)},
      {"HTTP", "POST", "/reverse", RH(post_reverse_handler)},
      {"HTTP", "POST", "/play", RH(post_play_handler)},
      {"HTTP", "POST", "/scrub", RH(post_scrub_handler)},
      {"HTTP", "POST", "/rate", RH(post_rate_handler)},
      {"HTTP", "POST", "/stop", RH(post_stop_handler)},
      {"HTTP", "POST", "/action", RH(post_action_handler)},
      {"HTTP", "GET", "/playback-info", RH(get_playback_info_handler)},
      {"HTTP", "PUT", "/setProperty", RH(put_setProperty_handler)},
      {"HTTP", "POST", "/getProperty", RH(post_getProperty_handler)},
  };

  // Register all the request handlers
  for (auto route : routes_table) {
    register_request_route(route);
  }
}

void ap_airplay_connection::send_fcup_request(int request_id,
                                              const std::string &url,
                                              const std::string &session_id) {
  std::ostringstream oss;
  // clang-format off
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
           "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
           "<plist version=\"1.0\">\n"
           "\t<dict>\n"
           "\t\t<key>sessionID</key>\n"
           "\t\t<integer>1</integer>\n"
           "\t\t<key>type</key>\n"
           "\t\t<string>unhandledURLRequest</string>\n"
           "\t\t<key>request</key>\n"
           "\t\t<dict>\n"
           "\t\t\t<key>FCUP_Response_ClientInfo</key>\n"
           "\t\t\t<integer>1</integer>\n"
           "\t\t\t<key>FCUP_Response_ClientRef</key>\n"
           "\t\t\t<integer>40030004</integer>\n"
           "\t\t\t<key>FCUP_Response_RequestID</key>\n"
           "\t\t\t<integer>" << request_id << "</integer>\n"
           "\t\t\t<key>FCUP_Response_URL</key>\n"
           "\t\t\t<string>" << url << "</string>\n"
           "\t\t\t<key>sessionID</key>\n"
           "\t\t\t<integer>1</integer>\n"
           "\t\t\t<key>FCUP_Response_Headers</key>\n"
           "\t\t\t<dict>\n"
           "\t\t\t\t<key>X-Playback-Session-Id</key>\n"
           "\t\t\t\t<string>" << session_id << "</string>\n"
           "\t\t\t\t<key>User-Agent</key>\n"
           "\t\t\t\t<string>AppleCoreMedia/1.0.0.11B554a (Apple TV; U; CPU OS 7_0_4 like Mac OS X; en_us)</string>\n"
           "\t\t\t</dict>\n"
           "\t\t</dict>\n"
           "\t</dict>\n"
           "</plist>";
  // clang-format on

  request fcup_request("HTTP/1.1", "POST", "/event");
  fcup_request.with_header("X-Apple-Session-ID", session_id)
      .with_content_type(TEXT_APPLE_PLIST_XML)
      .with_content(oss.str());

  auto p = ap_event_connection_manager::get().get(session_id);
  auto pp = p.lock();
  if (pp) {
    pp->send_request(fcup_request);
  }
}

void ap_airplay_connection::reverse_connection(const std::string &session) {
  apple_session_id_ = session;
  ap_event_connection_manager::get().insert(session, shared_from_self());
  reverse();
}

ap_airplay_service::ap_airplay_service(ap_config_ptr &config,
                                       uint16_t port /*= 0*/)
    : tcp_service_base("ap_airplay_service", port), config_(config) {
  bind_thread_actions(std::bind(&ap_airplay_service::on_thread_start, this),
                      std::bind(&ap_airplay_service::on_thread_stop, this));
}

ap_airplay_service::~ap_airplay_service() {}

void ap_airplay_service::set_handler(ap_handler_ptr &hanlder) {
  handler_ = hanlder;
}

tcp_connection_ptr ap_airplay_service::prepare_new_connection() {
  return std::make_shared<ap_airplay_connection>(io_context(), config_,
                                                 handler_, shared_from_this());
}

void ap_airplay_service::on_thread_start() {
  if (handler_) {
    handler_->on_thread_start();
  }
}

void ap_airplay_service::on_thread_stop() {
  if (handler_) {
    handler_->on_thread_stop();
  }
}

} // namespace service
} // namespace aps
