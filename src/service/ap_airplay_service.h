#pragma once
#include <array>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <asio.hpp>

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

using namespace aps::network;

namespace aps {
namespace service {
class ap_airplay_connection
    : public xtxp_connection_base,
      public ap_session,
      public std::enable_shared_from_this<ap_airplay_connection> {
public:
  explicit ap_airplay_connection(asio::io_context &io_ctx,
                                 ap_config_ptr &config, ap_handler_ptr &handler,
                                 tcp_service_weak_ptr service);

  ~ap_airplay_connection();

  virtual uint64_t get_session_id() override;

  virtual uint32_t get_session_type() override;

  virtual void disconnect() override;

  virtual void
  set_mirror_handler(ap_mirror_session_handler_ptr handler) override;

  virtual void set_video_handler(ap_video_session_handler_ptr handler) override;

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
  void get_server_info_handler(const request &req, response &res);

  void post_fp_setup2_handler(const request &req, response &res);

  void post_reverse_handler(const request &req, response &res);

  // SDK -> APP
  void post_play_handler(const request &req, response &res);

  // SDK -> APP
  void post_scrub_handler(const request &req, response &res);

  // SDK -> APP
  void post_rate_handler(const request &req, response &res);

  // SDK -> APP
  void post_stop_handler(const request &req, response &res);

  // SDK -> APP
  void post_action_handler(const request &req, response &res);

  // SDK -> APP
  void get_playback_info_handler(const request &req, response &res);

  // SDK -> APP
  void put_setProperty_handler(const request &req, response &res);

  // APP -> SDK
  void post_getProperty_handler(const request &req, response &res);

  virtual std::shared_ptr<xtxp_connection_base> shared_from_self() override;

protected:
  virtual void add_common_header(const request &req, response &res) override;

  void init_session_id();

  void validate_user_agent(const request &req);

  void initialize_request_handlers();

  void send_fcup_request(int request_id, const std::string &url,
                         const std::string &session_id);

  void reverse_connection(const std::string &session);

private:
  bool is_reversed_;
  uint64_t session_id_;
  uint32_t session_type_;
  std::string agent_;
  agent_version_t agent_version_;
  ap_config_ptr config_;
  ap_handler_ptr handler_;
  ap_crypto_ptr crypto_;
  ap_timing_sync_service_ptr timing_sync_service_;
  tcp_service_weak_ptr service_;

  // For mirroring
  // bool is_mirror_session_;
  ap_video_stream_service_ptr mirror_stream_service_;
  ap_audio_stream_service_ptr audio_stream_service_;
  ap_mirror_session_handler_ptr mirror_session_handler_;

  // For video streaming
  // bool is_video_session_;
  float start_pos_in_ms_;
  std::string playback_uuid_;
  std::string apple_session_id_;
  ap_video_session_handler_ptr video_session_handler_;
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
  ap_config_ptr config_;

  ap_handler_ptr handler_;
};

typedef std::shared_ptr<ap_airplay_service> ap_airplay_service_ptr;

} // namespace service
} // namespace aps
