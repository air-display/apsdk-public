#pragma once
#include <array>
#include <asio.hpp>
#include <map>
#include <network/tcp_service.h>
#include <network/xtxp_message.h>
#include <string>
#include <vector>


namespace aps {
namespace network {
class xtxp_connection_base
    : public tcp_connection_base,
      public std::enable_shared_from_this<xtxp_connection_base> {

  typedef std::function<void(const request &req, response &res)> request_hanlder;
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

  explicit xtxp_connection_base(asio::io_context &io_ctx);

  ~xtxp_connection_base();

  void register_rtsp_request_handler(request_hanlder handler,
                                     const std::string &method,
                                     const std::string &path = std::string());

  void register_http_request_handler(request_hanlder handler,
                                     const std::string &method,
                                     const std::string &path = std::string());

  void register_request_handler(service_type_t service, request_hanlder handler,
                                const std::string &method,
                                const std::string &path = std::string());

  void register_request_route(const request_route_t &route);

  virtual void start() override;

  virtual void add_common_header(const request &req, response &res);

 protected:

  void post_receive_message_head();

  void on_message_head_received(const asio::error_code &e,
                                std::size_t bytes_transferred);

  void post_receive_message_content();

  void on_message_content_received(const asio::error_code &e,
                                   std::size_t bytes_transferred);

  void post_send_response(const response &res);

  void on_response_sent(const asio::error_code &e,
                        std::size_t bytes_transferred);

  void send_request(const request &req);

  void handle_socket_error(const asio::error_code &e);

  std::size_t body_completion_condition(const asio::error_code &error,
                                        std::size_t bytes_transferred);

  void process_request();

  void process_response();

  void method_not_found_handler(const request &req,
                                response &res);

  void path_not_found_handler(const request &req,
                              response &res);

 private:
  bool is_reversed_;

  std::string agent_;

  std::string playback_uuid_;

  std::string apple_session_id_;

  asio::streambuf in_stream_;

  asio::streambuf out_stream_;

  request request_;

  response response_;

  http_message_parser parser_;

  request_handler_map rtsp_request_handlers_;
  request_handler_map http_request_handlers_;
};

typedef std::shared_ptr<xtxp_connection_base> xtxp_connection_base_ptr;
typedef std::weak_ptr<xtxp_connection_base> xtxp_connection_base_weak_ptr;
} // namespace network
} // namespace aps
