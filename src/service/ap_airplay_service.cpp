#include <ctime>
#include <exception>
#include <service/ap_airplay_service.h>
#include <service/ap_audio_stream_service.h>
#include <service/ap_content_parser.h>
#include <service/ap_types.h>
#include <service/ap_video_stream_service.h>
#include <stdexcept>
#include <string.h>
#include <utils/logger.h>
#include <utils/plist.h>
#include <utils/utils.h>


#if defined(NDEBUG)
#define DUMP_REQUEST_WITH_SESSION(x)
#else
#define DUMP_REQUEST_WITH_SESSION(x)                                           \
  x.dump("[" + std::to_string((long long)this) + "]")
#endif // _DEBUG

using namespace aps::service::details;

namespace aps {
namespace service {
ap_airplay_session::ap_airplay_session(
    asio::io_context &io_ctx, aps::ap_config_ptr &config,
    aps::ap_handler_ptr &hanlder, aps::network::tcp_service_weak_ptr service)
    : tcp_session_base(io_ctx), config_(config), handler_(hanlder),
      service_(service), is_reversed_(false) {
  crypto_ = std::make_shared<ap_crypto>();

  timing_sync_service_ = std::make_shared<ap_timing_sync_service>();
  timing_sync_service_->open();

  initialize_request_handlers();

  LOGD() << "ap_airplay_session (" << std::hex << this << ") is being created";
}

ap_airplay_session::~ap_airplay_session() {
  if (timing_sync_service_) {
    timing_sync_service_->close();
    timing_sync_service_.reset();
  }

  if (is_reversed_) {
    ap_event_sesions::instance().remove(apple_session_id_);
  }

  LOGD() << "ap_airplay_session (" << std::hex << this
         << ") is being destroyed";
}

void ap_airplay_session::register_rtsp_request_handler(
    request_hanlder handler, const std::string &method,
    const std::string &path /*= std::string()*/) {
  auto path_map = rtsp_request_handlers_.find(method);
  if (path_map == rtsp_request_handlers_.end()) {
    path_handler_map path_map;
    path_map[path] = handler;
    rtsp_request_handlers_[method] = path_map;
  } else {
    path_map->second[path] = handler;
  }
}

void ap_airplay_session::register_http_request_handler(
    request_hanlder handler, const std::string &method,
    const std::string &path /*= std::string()*/) {
  auto path_map = http_request_handlers_.find(method);
  if (path_map == http_request_handlers_.end()) {
    path_handler_map path_map;
    path_map[path] = handler;
    http_request_handlers_[method] = path_map;
  } else {
    path_map->second[path] = handler;
  }
}

void ap_airplay_session::register_request_handler(
    service_type_t service, request_hanlder handler, const std::string &method,
    const std::string &path /*= std::string()*/) {
  if (RTSP == service) {
    register_rtsp_request_handler(handler, method, path);
  } else if (HTTP == service) {
    register_http_request_handler(handler, method, path);
  } else
    return;
}

void ap_airplay_session::register_request_route(const request_route_t &route) {
  register_request_handler(route.service, route.handler, route.method,
                           route.path);
}

void ap_airplay_session::start() { post_receive_message_head(); }

void ap_airplay_session::options_handler(const details::request &req,
                                         details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

  res.with_status(ok)
      .with_header("Public", "ANNOUNCE, SETUP, RECORD, PAUSE, FLUSH, TEARDOWN, "
                             "OPTIONS, GET_PARAMETER, SET_PARAMETER, POST, GET")
      .with_content_type(APPLICATION_OCTET_STREAM);
}

void ap_airplay_session::post_pair_setup_handler(const details::request &req,
                                                 details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

  if (32 == req.content.size()) {
    crypto_->init_client_public_keys(0, 0, req.content.data(), 32);
  }

  res.with_status(ok)
      .with_content(crypto_->server_keys().ed_public_key())
      .with_content_type(APPLICATION_OCTET_STREAM);
}

void ap_airplay_session::post_pair_verify_handler(const details::request &req,
                                                  details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

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

void ap_airplay_session::post_fp_setup_handler(const details::request &req,
                                               details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

  fp_header_t *header = (fp_header_t *)req.content.data();
  if (header->major_version == 0x03) {
    if (header->phase == 0x01 && req.content.size() == 16) {
      uint8_t mode = req.content[14];
      std::vector<uint8_t> content(142, 0);
      crypto_->fp_setup(mode, content.data());
      res.with_status(ok)
          .with_content_type(APPLICATION_OCTET_STREAM)
          .with_content(content);
    } else if (header->phase == 0x03 && req.content.size() == 164) {
      std::vector<uint8_t> content(32, 0);
      crypto_->fp_handshake(content.data(), (uint8_t *)&req.content[144]);
      res.with_status(ok)
          .with_content_type(APPLICATION_OCTET_STREAM)
          .with_content(content);
    } else {
      LOGE() << "Invalid request";
      res.with_status(bad_request);
      return;
    }
  }

  res.with_status(ok).with_content_type(APPLICATION_OCTET_STREAM);
}

void ap_airplay_session::setup_handler(const details::request &req,
                                       details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

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
        crypto_->init_audio_stream_aes_cbc();

        if (!audio_stream_service_) {
          audio_stream_service_ =
              std::make_shared<ap_audio_stream_service>(crypto_, handler_);
          audio_stream_service_->start();
        }

        if (handler_) {
          handler_->on_audio_stream_started();
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
        auto connection_id_obj =
            plist_object_dict_get_value(stream_obj, "streamConnectionID");
        if (PLIST_TYPE_INTEGER != plist_object_get_type(connection_id_obj))
          break;

        int64_t connection_id = 0;
        if (0 !=
            plist_object_integer_get_value(connection_id_obj, &connection_id))
          break;

        crypto_->init_video_stream_aes_ctr(connection_id);

        if (!video_stream_service_) {
          video_stream_service_ =
              std::make_shared<ap_video_stream_service>(crypto_, 0, handler_);
          video_stream_service_->start();
        }

        if (handler_) {
          handler_->on_mirror_stream_started();
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
                    "dataPort", plist_object_integer(video_stream_service_->port())
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

void ap_airplay_session::get_info_handler(const details::request &req,
                                          details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

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
              "inputLatencyMicros", plist_object_integer(3),
              "outputLatencyMicros", plist_object_integer(79)
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
              "overscanned", plist_object_false(), 
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

void ap_airplay_session::post_feedback_handler(const details::request &req,
                                               details::response &res) {
  res.with_status(ok);
}

void ap_airplay_session::record_handler(const details::request &req,
                                        details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

  timing_sync_service_->post_send_query();

  res.with_status(ok).with_header("Audio-Latency", "0");
}

void ap_airplay_session::get_parameter_handler(const details::request &req,
                                               details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

  res.with_status(ok)
      .with_content_type(TEXT_PARAMETERS)
      .with_content("volume: 0.000000\r\n");
}

void ap_airplay_session::post_audioMode(const details::request &req,
                                        details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

  res.with_status(ok);
}

void ap_airplay_session::set_parameter_handler(const details::request &req,
                                               details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);
  std::string content(req.content.begin(), req.content.end());
  if (0 == req.content_type.compare(TEXT_PARAMETERS)) {
    float ratio = 0;
    float volume = 0;
    uint64_t start = 0;
    uint64_t current = 0;
    uint64_t end = 0;

    // volume: -11.123877
    if (ap_content_parser::get_volume_value(volume, content.c_str())) {
      if (handler_) {
        if (0 == volume)
          ratio = 100;
        else if (-144.0f == volume)
          ratio = 0;
        else {
          ratio = 100.0f * (30 + volume) / 30;
        }

        handler_->on_audio_set_volume(ratio, volume);
      }
    }
    // progress: 1146221540/1146549156/1195701740  start/current/end
    else if (ap_content_parser::get_progress_values(start, current, end,
                                                    content.c_str())) {
      if (handler_) {
        ratio = 100.0f * (current - start) / (end - start);
        handler_->on_audio_set_progress(ratio, start, current, end);
      }
    }
  } else if (0 == req.content_type.compare(IMAGE_JPEG) ||
             0 == req.content_type.compare(IMAGE_PNG)) {
    // body is image data
    if (handler_) {
      handler_->on_audio_set_cover(req.content_type, req.content.data(),
                                   req.content.size());
    }
  } else if (0 == req.content_type.compare(APPLICATION_DMAP_TAGGED)) {
    // body is dmap data
    if (handler_) {
      handler_->on_audio_set_meta_data(req.content.data(), req.content.size());
    }
  } else {
    LOGE() << "Unknown parameter type: " << req.content_type;
  }

  res.with_status(ok);
}

void ap_airplay_session::teardown_handler(const details::request &req,
                                          details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

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
              if (video_stream_service_) {
                video_stream_service_->stop();
                video_stream_service_.reset();
              }

              if (handler_) {
                handler_->on_mirror_stream_stopped();
              }

              LOGD() << "Mirroring video stream disconnected";
            } else if (stream_type_t::audio == type) {
              // Stop audio stream
              if (audio_stream_service_) {
                audio_stream_service_->stop();
                audio_stream_service_.reset();
              }

              if (handler_) {
                handler_->on_audio_stream_stopped();
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

void ap_airplay_session::flush_handler(const details::request &req,
                                       details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

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

void ap_airplay_session::get_server_info(const details::request &req,
                                         details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);
  res.with_status(ok);
  // return;

  std::ostringstream oss;
  // clang-format off
  oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
         "<plist version=\"1.0\">\n"
         "<dict>\n"
         "\t<key>features</key>\n"
         "\t<integer>" /*<< config_->features()*/"0x27F" << "</integer>\n"
         "\t<key>macAddress</key>\n"
         "\t<string>" << config_->macAddress() << "</string>\n"
         "\t<key>model</key>\n"
         "\t<string>" << /*config_->model()*/ "AppleTV2,1" << "</string>\n"
         "\t<key>osBuildVersion</key>\n"
         "\t<string>12B435</string>\n"
         "\t<key>protovers</key>\n"
         "\t<string>1.0</string>\n"
         "\t<key>srcvers</key>\n"
         "\t<string>" << /*config_->serverVersion()*/"101.28" << "</string>\n"
         "\t<key>vv</key>\n"
         "\t<integer>" << config_->vv() << "</integer>\n"
         "\t<key>deviceid</key>\n"
         "\t<string>" << config_->macAddress() << "</string>\n"
         "</dict>\n"
         "</plist>";
  // clang-format on

  res.with_status(ok)
      .with_content_type(TEXT_APPLE_PLIST_XML)
      .with_content(oss.str());
}

void ap_airplay_session::post_fp_setup2_handler(const details::request &req,
                                                details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

  res.with_status(ok);
}

void ap_airplay_session::post_reverse(const details::request &req,
                                      details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

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

  auto session_id = req.headers.find("X-Apple-Session-ID");
  if (session_id != req.headers.end()) {
    LOGD() << "Session_id: " << session_id->second;
  }

  ap_event_sesions::instance().insert(session_id->second, shared_from_this());
  is_reversed_ = true;

  res.with_status(switching_protocols);
}

void ap_airplay_session::post_play(const details::request &req,
                                   details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

  std::string location;
  float start_pos = 0.0f;

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

    double p = 0;
    auto start_position_obj =
        plist_object_dict_get_value(data_obj, "Start-Position");
    if (0 != plist_object_real_get_value(start_position_obj, &p)) {
      res.with_status(bad_request);
      return;
    }
    start_pos_ = p;
  } else if (0 == compare_string_no_case(req.content_type.c_str(),
                                         TEXT_PARAMETERS) ||
             0 == compare_string_no_case(req.content_type.c_str(),
                                         APPLICATION_OCTET_STREAM)) {
    // The request content is text:
    // req.body:
    //      Content-Location:URL
    //      Start-Position: float
    std::string content(req.content.begin(), req.content.end());
    if (!ap_content_parser::get_play_parameters(location, start_pos,
                                                content.c_str())) {
      LOGW() << "No start position found, set to 0";
    }
  } else {
    LOGW() << "Unknown content type :" << req.content_type
           << " for uri: " << req.uri;
    res.with_status(bad_request);
    return;
  }

  start_pos_ = start_pos;

  auto it_session_id = req.headers.find("X-Apple-Session-ID");
  if (it_session_id == req.headers.end()) {
    res.with_status(bad_request);
    return;
  }

  // Build FCUP request
  std::ostringstream oss;
  if (0 == location.find(MLHLS_SCHEME) || 0 == location.find(NFHLS_SCHEME)) {
    // Reset the fcup request id
    fcup_request_id_ = 1;
    send_fcup_request(fcup_request_id_++, location, it_session_id->second);
  } else {
    // Normal URL
    if (handler_) {
      handler_->on_video_play(location, start_pos);
    }
  }

  res.with_status(ok);
}

void ap_airplay_session::post_scrub(const details::request &req,
                                    details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);
  // /scrub?position=1298.000000
  float postition = 0.0f;
  if (ap_content_parser::get_scrub_position(postition, req.uri.c_str())) {
    if (handler_) {
      handler_->on_video_scrub(postition);
    }
  }

  res.with_status(ok);
}

void ap_airplay_session::post_rate(const details::request &req,
                                   details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);
  // /rate?value=0.000000
  float value = 0.0f;
  if (ap_content_parser::get_rate_value(value, req.uri.c_str())) {
    if (handler_) {
      handler_->on_video_rate(value);
    }
  }

  res.with_status(ok);
}

void ap_airplay_session::post_stop(const details::request &req,
                                   details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

  if (handler_) {
    handler_->on_video_stop();
  }

  res.with_status(ok);
}

void ap_airplay_session::post_action(const details::request &req,
                                     details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);
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

  if (0 == compare_string_no_case(type, "playlistInster")) {
    LOGD() << "Action type: " << type << ". Add new playback.";
  } else if (0 == compare_string_no_case(type, "playlistRemove")) {
    LOGD() << "Action type: " << type << ". Stop playback.";
  } else if (0 == compare_string_no_case(type, "unhandledURLResponse")) {
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
    auto fcup_data_obj =
        plist_object_dict_get_value(params_dict, "FCUP_Response_Data");
    const uint8_t *fcup_data = 0;
    uint64_t data_len = 0;
    if (0 !=
        plist_object_data_get_value(fcup_data_obj, &fcup_data, &data_len)) {
      res.with_status(bad_request);
      return;
    }

    std::string uri;
    if (get_youtube_url((char *)fcup_data, data_len, uri)) {
      if (handler_) {
        handler_->on_video_play(uri, start_pos_);
      }
    } else {
      uri = get_best_quality_stream_uri((char *)fcup_data, data_len);

      auto it_session_id = req.headers.find("X-Apple-Session-ID");
      if (it_session_id == req.headers.end()) {
        res.with_status(bad_request);
        return;
      }

      send_fcup_request(fcup_request_id_++, uri, it_session_id->second);

      res.with_content_type(TEXT_APPLE_PLIST_XML)
          .with_content(ERROR_STATUS_RESPONSE);
    }
  } else {
    LOGW() << "Unknown action type: " << type;
  }

  res.with_status(ok);
}

void ap_airplay_session::get_playback_info(const details::request &req,
                                           details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

  ap_handler::playback_info_t playbackinfo;
  if (handler_) {
    handler_->on_acquire_playback_info(playbackinfo);
  }
  LOGI() << "PlaybackInfo duration: " << playbackinfo.duration
         << ", position: " << playbackinfo.position;
  // Return binary plist
  // clang-format off
  // auto_plist info = plist_object_dict(10,
  //     //"uuid", plist_object_string(""),
  //     "stallCount", plist_object_integer(0),
  //     "duration", plist_object_real(playbackinfo.duration),
  //     "position", plist_object_real(playbackinfo.position),
  //     "rate", plist_object_real(playbackinfo.rate),
  //     "readyToPlay", plist_object_integer(1),
  //     "playbackBufferEmpty", plist_object_integer(1),
  //     "playbackBufferFull", plist_object_integer(0),
  //     "playbackLikelyToKeepUp", plist_object_integer(1),
  //     "loadedTimeRanges", plist_object_array(1, 
  //         plist_object_dict(2,
  //             "start", plist_object_real(0),
  //             "duration", plist_object_real(playbackinfo.duration)
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
         "\t<real>" << playbackinfo.duration << "</real>\n"
         "\t<key>loadedTimeRanges</key>\n"
         "\t<array>\n"
         "\t\t<dict>\n"
         "\t\t\t<key>duration</key>\n"
         "\t\t\t<real>" << playbackinfo.duration << "</real>\n"
         "\t\t\t<key>start</key>\n"
         "\t\t\t<real>0.0</real>\n"
         "\t\t</dict>\n"
         "\t</array>\n"
         "\t<key>playbackBufferEmpty</key>\n"
         "\t<integer>1</integer>\n"
         "\t<key>playbackBufferFull</key>\n"
         "\t<integer>0</integer>\n"
         "\t<key>playbackLikelyToKeepUp</key>\n"
         "\t<integer>1</integer>\n"
         "\t<key>position</key>\n"
         "\t<real>" << playbackinfo.position << "</real>\n"
         "\t<key>rate</key>\n"
         "\t<real>" << playbackinfo.rate << "</real>\n"
         "\t<key>readyToPlay</key>\n"
         "\t<integer>" << playbackinfo.readyToPlay << "</integer>\n"
         "\t<key>seekableTimeRanges</key>\n"
         "\t<array>\n"
         "\t\t<dict>\n"
         "\t\t\t<key>duration</key>\n"
         "\t\t\t<real>0.0</real>\n"
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

void ap_airplay_session::put_setProperty(const details::request &req,
                                         details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);

  if (std::string::npos != req.uri.find("actionAtItemEnd")) {
    // /setProperty?actionAtItemEnd
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

void ap_airplay_session::post_getProperty(const details::request &req,
                                          details::response &res) {
  DUMP_REQUEST_WITH_SESSION(req);
  // GET /getProperty?playbackAccessLog
  // GET /getProperty?playbackErrorLog

  res.with_status(ok);
}

void ap_airplay_session::post_receive_message_head() {
  asio::async_read_until(
      socket_, in_stream_, RNRN_LINE_BREAK,
      asio::bind_executor(
          strand_, std::bind(&ap_airplay_session::on_message_head_received,
                             shared_from_this(), std::placeholders::_1,
                             std::placeholders::_2)));
}

void ap_airplay_session::on_message_head_received(
    const asio::error_code &e, std::size_t bytes_transferred) {
  // If error then return
  if (e) {
    handle_socket_error(e);
    return;
  }

  std::string head_data(asio::buffers_begin(in_stream_.data()),
                        asio::buffers_begin(in_stream_.data()) +
                            bytes_transferred);
  in_stream_.consume(bytes_transferred);

  bool parse_result = false;
  if (is_reversed_) {
    // Parse the request head
    parse_result = parser_.parse(response_, head_data);
  } else {
    // Parse the request head
    parse_result = parser_.parse(request_, head_data);
  }
  if (!parse_result) {
    // Invalid request head, close this session
    LOGE() << "Invalid message (neither request nor response)";
    stop();
    return;
  }

  int content_length =
      is_reversed_ ? response_.content_length : request_.content_length;

  if (content_length == 0) {
    // This is a head only message, process and prepare to read next one
    if (is_reversed_) {
      process_response();
    } else {
      process_request();
    }
    post_receive_message_head();
    return;
  }

  // The message has content data to be read
  if (content_length - in_stream_.size()) {
    post_receive_message_content();
    return;
  }

  if (is_reversed_) {
    // No more data to be read just build the message
    if (in_stream_.size()) {
      response_.content.clear();
      response_.content.resize(response_.content_length, 0);

      response_.content.assign(asio::buffers_begin(in_stream_.data()),
                               asio::buffers_begin(in_stream_.data()) +
                                   in_stream_.size());
    }
  } else {
    // No more data to be read just build the message
    if (in_stream_.size()) {
      request_.content.clear();
      request_.content.resize(request_.content_length, 0);

      request_.content.assign(asio::buffers_begin(in_stream_.data()),
                              asio::buffers_begin(in_stream_.data()) +
                                  in_stream_.size());
    }
  }
  in_stream_.consume(in_stream_.size());

  if (is_reversed_) {
    process_response();
  } else {
    process_request();
  }
  // Prepare to read next message
  post_receive_message_head();
}

void ap_airplay_session::post_receive_message_content() {
  if (is_reversed_) {
    response_.content.clear();
  } else {
    request_.content.clear();
  }

  asio::async_read(
      socket_, in_stream_,
      std::bind(&ap_airplay_session::body_completion_condition,
                shared_from_this(), std::placeholders::_1,
                std::placeholders::_2),
      asio::bind_executor(
          strand_, std::bind(&ap_airplay_session::on_message_content_received,
                             shared_from_this(), std::placeholders::_1,
                             std::placeholders::_2)));
}

void ap_airplay_session::on_message_content_received(
    const asio::error_code &e, std::size_t bytes_transferred) {
  if (e) {
    handle_socket_error(e);
    return;
  }

  if (is_reversed_) {
    response_.content.assign(asio::buffers_begin(in_stream_.data()),
                             asio::buffers_begin(in_stream_.data()) +
                                 in_stream_.size());
    in_stream_.consume(in_stream_.size());
  } else {
    request_.content.assign(asio::buffers_begin(in_stream_.data()),
                            asio::buffers_begin(in_stream_.data()) +
                                in_stream_.size());
    in_stream_.consume(in_stream_.size());
  }

  if (is_reversed_) {
    process_response();
  } else {
    process_request();
  }
  post_receive_message_head();
}

void ap_airplay_session::post_send_response(const details::response &res) {
  res.format_to_stream(out_stream_);
  asio::async_write(
      socket_, out_stream_,
      asio::bind_executor(strand_,
                          std::bind(&ap_airplay_session::on_response_sent,
                                    shared_from_this(), std::placeholders::_1,
                                    std::placeholders::_2)));
}

void ap_airplay_session::on_response_sent(const asio::error_code &e,
                                          std::size_t bytes_transferred) {
  if (!e) {
    LOGV() << ">>>>> " << bytes_transferred << " bytes sent successfully";
  } else {
    handle_socket_error(e);
  }
}

void ap_airplay_session::send_request(const details::request &req) {
  std::string data = req.format_to_string();
  socket_.send(asio::buffer(data.data(), data.length()));
}

void ap_airplay_session::add_common_header(const details::request &req,
                                           details::response &res) {
  res.with_header(HEADER_SERVER, "AirTunes/220.68")
      .with_header(HEADER_SESSION, "CAFEBABE");

  if (!req.cseq.empty())
    res.with_header(HEADER_CSEQ, req.cseq);

  if (0 != req.method.compare("RECORD") &&
      0 != req.method.compare("SET_PARAMETER"))
    res.with_header(HEADER_AUDIO_JACK_STATUS, "Connected; type=digital");
}

void ap_airplay_session::handle_socket_error(const asio::error_code &e) {
  switch (e.value()) {
  case asio::error::eof:
    return;
  case asio::error::connection_reset:
  case asio::error::connection_aborted:
  case asio::error::access_denied:
  case asio::error::address_family_not_supported:
  case asio::error::address_in_use:
  case asio::error::already_connected:
  case asio::error::connection_refused:
  case asio::error::bad_descriptor:
  case asio::error::fault:
  case asio::error::host_unreachable:
  case asio::error::in_progress:
  case asio::error::interrupted:
  case asio::error::invalid_argument:
  case asio::error::message_size:
  case asio::error::name_too_long:
  case asio::error::network_down:
  case asio::error::network_reset:
  case asio::error::network_unreachable:
  case asio::error::no_descriptors:
  case asio::error::no_buffer_space:
  case asio::error::no_protocol_option:
  case asio::error::not_connected:
  case asio::error::not_socket:
  case asio::error::operation_not_supported:
  case asio::error::shut_down:
  case asio::error::timed_out:
  case asio::error::would_block:
    break;
  }

  LOGE() << "Socket error[" << e.value() << "]: " << e.message();
}

std::size_t
ap_airplay_session::body_completion_condition(const asio::error_code &error,
                                              std::size_t bytes_transferred) {
  return request_.content_length - in_stream_.size();
}

void ap_airplay_session::validate_user_agent() {
  if (!agent_.empty())
    return;

  auto user_agent_header = request_.headers.find("User-Agent");
  if (user_agent_header != request_.headers.end()) {
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

void ap_airplay_session::process_request() {
  response res(request_.scheme_version);

  validate_user_agent();

  request_handler_map *handler_map = 0;
  if (0 == request_.scheme_version.find("RTSP"))
    handler_map = &rtsp_request_handlers_;
  else if (0 == request_.scheme_version.find("HTTP"))
    handler_map = &http_request_handlers_;
  else
    return;

  auto method_to_path = handler_map->find(request_.method);
  if (method_to_path == handler_map->end())
    method_not_found_handler(request_, res);
  else {
    auto path_to_handler = method_to_path->second.find("*");
    if (path_to_handler == method_to_path->second.end()) {
      std::string path = request_.uri;
      auto index = request_.uri.find('?');
      if (std::string::npos != index)
        path = request_.uri.substr(0, index);

      path_to_handler = method_to_path->second.find(path);
    }

    if (path_to_handler == method_to_path->second.end())
      path_not_found_handler(request_, res);
    else
      path_to_handler->second(request_, res);
  }

  res.with_header(HEADER_DATE, gmt_time_string());

  if (!res.no_common_headers()) {
    add_common_header(request_, res);
  }

  post_send_response(res);
}

void ap_airplay_session::process_response() {}

#define RH(x)                                                                  \
  std::bind(&ap_airplay_session::x, this, std::placeholders::_1,               \
            std::placeholders::_2)

void ap_airplay_session::initialize_request_handlers() {
  // The request route table
  request_route_t routes_table[] = {
      {RTSP, "OPTIONS", "", RH(options_handler)},
      {RTSP, "POST", "/pair-setup", RH(post_pair_setup_handler)},
      {RTSP, "POST", "/pair-verify", RH(post_pair_verify_handler)},
      {RTSP, "POST", "/fp-setup", RH(post_fp_setup_handler)},
      {RTSP, "SETUP", "*", RH(setup_handler)},
      {RTSP, "GET", "/info", RH(get_info_handler)},
      {RTSP, "POST", "/feedback", RH(post_feedback_handler)},
      {RTSP, "RECORD", "*", RH(record_handler)},
      {RTSP, "GET_PARAMETER", "*", RH(get_parameter_handler)},
      {RTSP, "SET_PARAMETER", "*", RH(set_parameter_handler)},
      {RTSP, "TEARDOWN", "*", RH(teardown_handler)},
      {RTSP, "FLUSH", "*", RH(flush_handler)},
      {RTSP, "POST", "/audioMode", RH(post_audioMode)},
      {HTTP, "GET", "/server-info", RH(get_server_info)},
      {HTTP, "POST", "/fp-setup", RH(post_fp_setup_handler)},
      {HTTP, "POST", "/fp-setup2", RH(post_fp_setup2_handler)},
      {HTTP, "POST", "/reverse", RH(post_reverse)},
      {HTTP, "POST", "/play", RH(post_play)},
      {HTTP, "POST", "/scrub", RH(post_scrub)},
      {HTTP, "POST", "/rate", RH(post_rate)},
      {HTTP, "POST", "/stop", RH(post_stop)},
      {HTTP, "POST", "/action", RH(post_action)},
      {HTTP, "GET", "/playback-info", RH(get_playback_info)},
      {HTTP, "PUT", "/setProperty", RH(put_setProperty)},
      {HTTP, "POST", "/getProperty", RH(post_getProperty)},
  };

  // Register all the request handlers
  for (auto route : routes_table) {
    register_request_route(route);
  }
}

void ap_airplay_session::send_fcup_request(int request_id,
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

  auto p = ap_event_sesions::instance().get(session_id);
  auto pp = p.lock();
  if (pp) {
    pp->send_request(fcup_request);
  }
}

void ap_airplay_session::method_not_found_handler(const details::request &req,
                                                  details::response &res) {
  LOGE() << "***** Method Not Allowed " << request_.method << " "
         << request_.uri;

  // Method not found
  // res.with_status(method_not_allowed);
  res.with_status(ok);
}

void ap_airplay_session::path_not_found_handler(const details::request &req,
                                                details::response &res) {
  LOGE() << "***** Path Not Found " << request_.method << " " << request_.uri;

  // Path not found
  // res.with_status(not_found);
  res.with_status(ok);
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

aps::network::tcp_session_ptr ap_airplay_service::prepare_new_session() {
  return std::make_shared<ap_airplay_session>(io_context(), config_, handler_,
                                              shared_from_this());
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

aps::service::ap_event_sesions &ap_event_sesions::instance() {
  static ap_event_sesions s_instance;
  return s_instance;
}

void ap_event_sesions::insert(const std::string &id,
                              ap_airplay_session_weak_ptr p) {
  std::lock_guard<std::mutex> l(mtx_);
  even_session_map_[id] = p;
}

void ap_event_sesions::remove(const std::string &id) {
  std::lock_guard<std::mutex> l(mtx_);
  auto it = even_session_map_.find(id);
  if (it != even_session_map_.end()) {
    even_session_map_.erase(it);
  }
}

aps::service::ap_airplay_session_weak_ptr
ap_event_sesions::get(const std::string &id) {
  std::lock_guard<std::mutex> l(mtx_);
  auto it = even_session_map_.find(id);
  if (it != even_session_map_.end()) {
    return it->second;
  }
  return ap_airplay_session_weak_ptr();
}

ap_event_sesions::ap_event_sesions() {}

ap_event_sesions::~ap_event_sesions() {}

} // namespace service
} // namespace aps
