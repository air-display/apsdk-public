#pragma once
#include <ap_handler.h>
#include <ap_types.h>
#include <ap_session.h>
#include <asio.hpp>
#include <crypto/ap_crypto.h>
#include <memory>
#include <network/udp_service.h>
#include <service/ap_audio_stream_service_details.h>
#include <utils/packing.h>
#include <vector>

using namespace aps::service::audio::details;

namespace aps {
namespace service {
class audio_udp_service : public aps::network::udp_service_base {
public:
  typedef std::function<void(const uint8_t *, const asio::error_code &,
                             std::size_t)>
      recv_from_handler;

  explicit audio_udp_service(const std::string &name);

  ~audio_udp_service();

  virtual bool open() override;

  void bind_recv_handler(recv_from_handler handler) {
    recv_from_handler_ = handler;
  }

  virtual void on_recv_from(asio::ip::udp::endpoint &remote_endpoint,
                            const asio::error_code &e,
                            std::size_t bytes_transferred) override;

protected:
  void handle_socket_error(const asio::error_code &e);

private:
  std::vector<uint8_t> recv_buf_;

  recv_from_handler recv_from_handler_;

  asio::ip::udp::endpoint remote_endpoint_;
};

class ap_audio_stream_service {
public:
  explicit ap_audio_stream_service(aps::ap_crypto_ptr &crypto,
                                  aps::ap_mirror_session_handler_ptr &handler);

  ~ap_audio_stream_service();

  uint16_t data_port() const;

  uint16_t control_port() const;

  bool start();

  void stop();

protected:
  void data_handler(const uint8_t *buf, const asio::error_code &e,
                    std::size_t bytes_transferred);

  void audio_data_packet(rtp_audio_data_packet_t *packet, size_t length);

  void control_handler(const uint8_t *buf, const asio::error_code &e,
                       std::size_t bytes_transferred);

  void control_sync_packet(rtp_control_sync_packet_t *packet);

  void control_retransmit_packet(rtp_control_retransmit_reply_packet_t *packet);

  void on_thread_start();

  void on_thread_stop();

private:
  aps::ap_mirror_session_handler_ptr handler_;

  aps::ap_crypto_ptr crypto_;

  audio_udp_service data_service_;

  audio_udp_service control_service_;
};

typedef std::shared_ptr<ap_audio_stream_service> ap_audio_stream_service_ptr;
} // namespace service
} // namespace aps
