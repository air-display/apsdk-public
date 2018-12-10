#pragma once
#include <ap_handler.h>
#include <crypto/ap_crypto.h>
#include <network/tcp_service.h>
#include <service/ap_mirror_stream_service_details.h>
#include <utils/packing.h>
#include <asio.hpp>
#include <memory>
#include <vector>
#include <fstream>

#define PERSIST_VIDEO_DATA_TO_FILE 1

using namespace aps::service::mirror::details;

namespace aps {
namespace service {
class ap_mirror_stream_connection
    : public aps::network::tcp_connection_base,
      public std::enable_shared_from_this<ap_mirror_stream_connection> {
 public:
  ap_mirror_stream_connection(asio::io_context &io_ctx,
                              aps::ap_crypto_ptr &crypto,
                              aps::ap_handler_ptr handler = 0);

  ~ap_mirror_stream_connection();

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

  std::vector<uint8_t> buffer_;

  sms_packet_header_t *header_;

  uint8_t *payload_;

  #if defined(WIN32) && PERSIST_VIDEO_DATA_TO_FILE
  std::ofstream video_data_file_;
  void init_video_data_file(sms_video_codec_packet_t *p);
  void append_nalu(sms_video_data_packet_t *p);
  void close_video_data_file();
#endif
};

class ap_mirror_stream_service : public aps::network::tcp_service_base {
 public:
  explicit ap_mirror_stream_service(aps::ap_crypto_ptr &crypto, uint16_t port,
                                    aps::ap_handler_ptr &handler);

  ~ap_mirror_stream_service();

 protected:
  virtual aps::network::tcp_connection_ptr prepare_new_connection() override;

  void on_thread_start();

  void on_thread_stop();

 private:
  aps::ap_handler_ptr handler_;

  aps::ap_crypto_ptr crypto_;
};

typedef std::shared_ptr<ap_mirror_stream_service> ap_video_stream_service_ptr;
}  // namespace service
}  // namespace aps
