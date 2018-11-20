#pragma once
#include <ap_config.h>
#include <ap_handler.h>
#include <crypto/ap_crypto.h>
#include <network/tcp_service.h>
#include <network/udp_service.h>
#include <service/ap_airplay_service_details.h>
#include <service/ap_audio_stream_service.h>
#include <service/ap_content_parser.h>
#include <service/ap_timing_sync_service.h>
#include <service/ap_video_stream_service.h>
#include <array>
#include <asio.hpp>
#include <map>
#include <string>
#include <vector>

namespace aps {
namespace service {
class ap_airplay_session
    : public aps::network::tcp_session_base,
      public std::enable_shared_from_this<ap_airplay_session> {
  typedef std::function<void(const details::request &req,
                             details::response &res)>
      request_hanlder;
  typedef std::map<std::string, request_hanlder> path_handler_map;
  typedef std::map<std::string, path_handler_map> request_handler_map;

 public:
  enum service_type_e { RTSP, HTTP };
  typedef service_type_e service_type_t;

  struct request_route_s {
    service_type_t service;
    std::string method;
    std::string path;
    request_hanlder handler;
  };
  typedef request_route_s request_route_t;

  explicit ap_airplay_session(asio::io_context &io_ctx,
                              aps::ap_config_ptr &config,
                              aps::ap_handler_ptr &handler);

  ~ap_airplay_session();

  void register_rtsp_request_handler(request_hanlder handler,
                                     const std::string &method,
                                     const std::string &path = std::string());

  void register_http_request_handler(request_hanlder handler,
                                     const std::string &method,
                                     const std::string &path = std::string());

  void register_request_handler(service_type_t service, 
                               request_hanlder handler,
                               const std::string &method,
                               const std::string &path = std::string());

  void register_request_route(const request_route_t &route);

  virtual void start() override;

 protected:
  // RTSP
  void options_handler(const details::request &req, details::response &res);

  void post_pair_setup_handler(const details::request &req,
                               details::response &res);

  void post_pair_verify_handler(const details::request &req,
                                details::response &res);

  void post_fp_setup_handler(const details::request &req,
                             details::response &res);

  void setup_handler(const details::request &req, details::response &res);

  void get_info_handler(const details::request &req, details::response &res);

  void post_feedback_handler(const details::request &req,
                             details::response &res);

  void record_handler(const details::request &req, details::response &res);

  void get_parameter_handler(const details::request &req,
                             details::response &res);

  void post_audioMode(const details::request &req, details::response &res);

  // SDK -> APP
  void set_parameter_handler(const details::request &req,
                             details::response &res);

  // SDK -> APP
  void teardown_handler(const details::request &req, details::response &res);

  // SDK -> APP
  void flush_handler(const details::request &req, details::response &res);

  // HTTP - Video
  void get_server_info(const details::request &req, details::response &res);

  // void post_fp_setup_handler_http(const details::request &req,
  //                                details::response &res);

  void post_fp_setup2_handler(const details::request &req,
                              details::response &res);

  void post_reverse(const details::request &req, details::response &res);

  // SDK -> APP
  void post_play(const details::request &req, details::response &res);

  // SDK -> APP
  void post_scrub(const details::request &req, details::response &res);

  // SDK -> APP
  void post_rate(const details::request &req, details::response &res);

  // SDK -> APP
  void post_stop(const details::request &req, details::response &res);

  // SDK -> APP
  void post_action(const details::request &req, details::response &res);

  // SDK -> APP
  void get_playback_info(const details::request &req, details::response &res);

  // SDK -> APP
  void put_setProperty(const details::request &req, details::response &res);

  // APP -> SDK
  void post_getProperty(const details::request &req, details::response &res);

 protected:
  void post_receive_request_head();

  void on_request_head_received(const asio::error_code &e,
                                std::size_t bytes_transferred);

  void post_receive_request_body();

  void on_request_body_received(const asio::error_code &e,
                                std::size_t bytes_transferred);

  void post_send_response(const details::response &res);

  void on_response_sent(const asio::error_code &e,
                        std::size_t bytes_transferred);

  void add_common_header(const details::request &req, details::response &res);

  void handle_socket_error(const asio::error_code &e);

  std::size_t body_completion_condition(const asio::error_code &error,
                                        std::size_t bytes_transferred);

  void validate_user_agent();

  void process_request();

  void method_not_found_handler(const details::request &req,
                                details::response &res);

  void path_not_found_handler(const details::request &req,
                              details::response &res);

  void initialize_request_handlers();

 private:
  std::string agent_;

  agent_version_t agent_version_;

  aps::ap_config_ptr config_;

  aps::ap_handler_ptr handler_;

  aps::ap_crypto_ptr crypto_;

  asio::streambuf in_stream_;

  asio::streambuf out_stream_;

  details::request request_;

  details::request_parser parser_;

  ap_timing_sync_service_ptr timing_sync_service_;

  ap_video_stream_service_ptr video_stream_service_;

  ap_audio_stream_service_ptr audio_stream_service_;

  request_handler_map rtsp_request_handlers_;

  request_handler_map http_request_handlers_;
};

typedef std::shared_ptr<ap_airplay_session> ap_airplay_session_ptr;

class ap_airplay_service : public aps::network::tcp_service_base {
 public:
  ap_airplay_service(ap_config_ptr &config, uint16_t port = 0);

  ~ap_airplay_service();

  void set_handler(ap_handler_ptr &hanlder);

 protected:
  virtual aps::network::tcp_session_ptr prepare_new_session() override;

  void on_thread_start();

  void on_thread_stop();

 private:
  aps::ap_config_ptr config_;
  aps::ap_handler_ptr handler_;
};

typedef std::shared_ptr<ap_airplay_service> ap_airplay_service_ptr;
}  // namespace service
}  // namespace aps
