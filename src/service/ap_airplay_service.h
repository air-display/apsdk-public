#pragma once
#include <ap_config.h>
#include <ap_handler.h>
#include <crypto/ap_crypto.h>
#include <network/tcp_service.h>
#include <network/udp_service.h>
#include <network/xtxp_connection_base.h>
#include <network/xtxp_message.h>
#include <service/ap_airplay_service_details.h>
#include <service/ap_audio_stream_service.h>
#include <service/ap_content_parser.h>
#include <service/ap_mirror_stream_service.h>
#include <service/ap_timing_sync_service.h>
#include <array>
#include <asio.hpp>
#include <map>
#include <string>
#include <vector>

using namespace aps::network;

namespace aps {
namespace service {
class ap_airplay_connection 
  : public xtxp_connection_base {
 public:
  explicit ap_airplay_connection(asio::io_context &io_ctx,
                                 aps::ap_config_ptr &config,
                                 aps::ap_handler_ptr &handler,
                                 tcp_service_weak_ptr service);

  ~ap_airplay_connection();

 protected:
  // RTSP
  void options_handler(const request &req, response &res);

  void post_pair_setup_handler(const request &req, response &res);

  void post_pair_verify_handler(const request &req, response &res);

  void post_fp_setup_handler(const request &req, response &res);

  void setup_handler(const request &req, response &res);

  void get_info_handler(const request &req, response &res);

  void post_feedback_handler(const request &req, response &res);

  void record_handler(const request &req, response &res);

  void get_parameter_handler(const request &req, response &res);

  void post_audioMode(const request &req, response &res);

  // SDK -> APP
  void set_parameter_handler(const request &req, response &res);

  // SDK -> APP
  void teardown_handler(const request &req, response &res);

  // SDK -> APP
  void flush_handler(const request &req, response &res);

  // HTTP - Video
  void get_server_info(const request &req, response &res);

  void post_fp_setup2_handler(const request &req, response &res);

  void post_reverse(const request &req, response &res);

  // SDK -> APP
  void post_play(const request &req, response &res);

  // SDK -> APP
  void post_scrub(const request &req, response &res);

  // SDK -> APP
  void post_rate(const request &req, response &res);

  // SDK -> APP
  void post_stop(const request &req, response &res);

  // SDK -> APP
  void post_action(const request &req, response &res);

  // SDK -> APP
  void get_playback_info(const request &req, response &res);

  // SDK -> APP
  void put_setProperty(const request &req, response &res);

  // APP -> SDK
  void post_getProperty(const request &req, response &res);

 protected:
  virtual void add_common_header(const request &req, response &res) override;

  void validate_user_agent(const request &req);

  void initialize_request_handlers();

  void send_fcup_request(int request_id, const std::string &url,
                         const std::string &session_id);

 private:
  float start_pos_;
  uint32_t fcup_request_id_;
  std::string agent_;
  std::string playback_uuid_;
  std::string apple_session_id_;
  agent_version_t agent_version_;
  aps::ap_config_ptr config_;
  aps::ap_handler_ptr handler_;
  aps::ap_crypto_ptr crypto_;
  ap_timing_sync_service_ptr timing_sync_service_;
  ap_video_stream_service_ptr mirror_stream_service_;
  ap_audio_stream_service_ptr audio_stream_service_;
  tcp_service_weak_ptr service_;
};

typedef std::shared_ptr<ap_airplay_connection> ap_airplay_connection_ptr;
typedef std::weak_ptr<ap_airplay_connection> ap_airplay_connection_weak_ptr;

/// <summary>
/// Represents the HTTP & RTSP server of the AirPlay receiver.
/// </summary>
class ap_airplay_service
    : public tcp_service_base,
      public std::enable_shared_from_this<ap_airplay_service> {
 public:
  ap_airplay_service(ap_config_ptr &config, uint16_t port = 0);

  ~ap_airplay_service();

  void set_handler(ap_handler_ptr &hanlder);

 protected:
  virtual tcp_connection_ptr prepare_new_connection() override;

  void on_thread_start();

  void on_thread_stop();

 private:
  aps::ap_config_ptr config_;

  aps::ap_handler_ptr handler_;
};

typedef std::shared_ptr<ap_airplay_service> ap_airplay_service_ptr;
}  // namespace service
}  // namespace aps
