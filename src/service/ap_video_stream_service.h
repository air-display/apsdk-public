#pragma once
#include <ap_handler.h>
#include <asio.hpp>
#include <crypto/ap_crypto.h>
#include <memory>
#include <network/tcp_service.h>
#include <service/ap_video_stream_service_details.h>
#include <utils/packing.h>
#include <vector>


using namespace aps::service::video::details;

namespace aps {
namespace service {
class ap_video_stream_session
    : public aps::network::tcp_session_base,
      public std::enable_shared_from_this<ap_video_stream_session> {
public:
  ap_video_stream_session(asio::io_context &io_ctx, aps::ap_crypto_ptr &crypto,
                          aps::ap_handler_ptr handler = 0);

  ~ap_video_stream_session();

  virtual void start() override;

protected:
  void post_receive_packet_header();

  void on_packet_header_received(const asio::error_code &e,
                                 std::size_t bytes_transferred);

  void post_receive_packet_payload();

  void on_packet_payload_received(const asio::error_code &e,
                                  std::size_t bytes_transferred);

  void process_packet();

  void handle_socket_error(const asio::error_code &e);

private:
  aps::ap_handler_ptr handler_;

  aps::ap_crypto_ptr crypto_;

  video::details::stream_packet_t packet_;
};

class ap_video_stream_service : public aps::network::tcp_service_base {
public:
  explicit ap_video_stream_service(aps::ap_crypto_ptr &crypto, uint16_t port,
                                   aps::ap_handler_ptr &handler);

  ~ap_video_stream_service();

protected:
  virtual aps::network::tcp_session_ptr prepare_new_session() override;

  void on_thread_start();

  void on_thread_stop();

private:
  aps::ap_handler_ptr handler_;

  aps::ap_crypto_ptr crypto_;
};

typedef std::shared_ptr<ap_video_stream_service> ap_video_stream_service_ptr;
} // namespace service
} // namespace aps
