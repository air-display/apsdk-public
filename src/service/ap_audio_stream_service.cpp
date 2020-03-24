#include <functional>

#include <crypto/ap_crypto.h>
#include <service/ap_audio_stream_service.h>
#include <utils/logger.h>

#define MAX_CACHED_PACKET_SIZE 10

using namespace aps::network;

namespace aps {
namespace service {
audio_udp_service::audio_udp_service(const std::string &name)
    : aps::network::udp_service_base(name), recv_buf_(RTP_PACKET_MAX_LEN, 0) {}

audio_udp_service::~audio_udp_service() {}

bool audio_udp_service::open() {
  if (aps::network::udp_service_base::open()) {
    post_recv_from(recv_buf_, remote_endpoint_);
    return true;
  }
  return false;
}

void audio_udp_service::on_recv_from(asio::ip::udp::endpoint &remote_endpoint,
                                     const asio::error_code &e,
                                     std::size_t bytes_transferred) {
  if (recv_from_handler_)
    recv_from_handler_(recv_buf_.data(), e, bytes_transferred);

  if (e)
    handle_socket_error(e);
  else
    post_recv_from(recv_buf_, remote_endpoint_);
}

void audio_udp_service::handle_socket_error(const asio::error_code &e) {
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

ap_audio_stream_service::ap_audio_stream_service(
    aps::ap_crypto_ptr &crypto, aps::ap_mirror_session_handler_ptr &handler)
    : handler_(handler), crypto_(crypto), data_service_("audio_data_service"),
      control_service_("audio_control_service"), expected_seq_(0) {
  data_service_.bind_recv_handler(std::bind(
      &ap_audio_stream_service::data_handler, this, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3));
  data_service_.bind_thread_actions(
      std::bind(&ap_audio_stream_service::on_thread_start, this),
      std::bind(&ap_audio_stream_service::on_thread_stop, this));

  control_service_.bind_recv_handler(std::bind(
      &ap_audio_stream_service::control_handler, this, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3));
  control_service_.bind_thread_actions(
      std::bind(&ap_audio_stream_service::on_thread_start, this),
      std::bind(&ap_audio_stream_service::on_thread_stop, this));

  LOGD() << "ap_audio_stream_service (" << std::hex << this
         << ") is being created";
}

ap_audio_stream_service::~ap_audio_stream_service() {
  LOGD() << "ap_audio_stream_service (" << std::hex << this
         << ") is being destroyed";
}

uint16_t ap_audio_stream_service::data_port() const {
  return data_service_.port();
}

uint16_t ap_audio_stream_service::control_port() const {
  return control_service_.port();
}

bool ap_audio_stream_service::start() {
  if (!data_service_.open())
    return false;

  if (!control_service_.open()) {
    data_service_.close();
    return false;
  }

  return true;
}

void ap_audio_stream_service::stop() {
  control_service_.close();
  data_service_.close();
}

void ap_audio_stream_service::data_handler(const uint8_t *buf,
                                           const asio::error_code &e,
                                           std::size_t bytes_transferred) {
  if (!e) {
    if (bytes_transferred < RTP_PACKET_MIN_LEN) {
      LOGE() << "Packet too small: " << bytes_transferred;
      return;
    }

    rtp_packet_header_t *header = (rtp_packet_header_t *)buf;
    header->sequence = ntohs(header->sequence);
    header->timestamp = ntohl(header->timestamp);
    if (header->payload_type != rtp_audio_data) {
      LOGE() << "Invalid audio data packet: " << bytes_transferred;
      return;
    }

    if (expected_seq_ == 0) {
      expected_seq_ = header->sequence;
    }

    // If the new packet is next expected one just process it
    if (header->sequence == expected_seq_) {
      audio_data_packet((rtp_audio_data_packet_t *)header, bytes_transferred);
      expected_seq_ = header->sequence + 1;
      // Check the cached buffer
      process_cached_packet();
      return;
    }

    // This packet is not the one we are waiting for
    if (header->sequence > expected_seq_) {
      if (cached_queue_.size() < MAX_CACHED_PACKET_SIZE) {
        LOGV() << "CACHE RTP PACKET +++++++++++++++++++++++"
               << "seq: " << header->sequence << ", expected:" << expected_seq_;
        // Cache this packet
        cache_packet(header->sequence, buf, bytes_transferred);
      } else {
        LOGV() << "FLUSH RTP PACKET ***********************"
               << "seq: " << header->sequence << ", expected:" << expected_seq_;
        // have been waiting for too long time, flush
        process_cached_packet(true);
      }
      return;
    }

    LOGV() << "ABANDON-RTP-PACKET xxxxxxxxxxxxxxxxxxxxxxx"
           << "seq: " << header->sequence << ", expected:" << expected_seq_;
  }
}

void ap_audio_stream_service::audio_data_packet(rtp_audio_data_packet_t *packet,
                                                size_t length) {
  LOGV() << "VALID RTP PACKET: " << length
         << ", sequence: " << packet->sequence;

  if (handler_) {
    uint32_t payload_length = length - sizeof(rtp_audio_data_packet_t);
    uint32_t encrypted_length = payload_length / 16 * 16;
    if (encrypted_length) {
      crypto_->decrypt_audio_data(packet->payload, encrypted_length);
    }
    handler_->on_audio_stream_data(packet, payload_length);
  }
}

void ap_audio_stream_service::control_handler(const uint8_t *buf,
                                              const asio::error_code &e,
                                              std::size_t bytes_transferred) {
  if (!e) {
    if (bytes_transferred < RTP_PACKET_MIN_LEN) {
      LOGE() << "Packet too small: " << bytes_transferred;
      return;
    }

    LOGV() << "ap_audio_stream_service::control_handler, " << bytes_transferred;

    rtp_packet_header_t *header = (rtp_packet_header_t *)buf;
    header->sequence = ntohs(header->sequence);
    header->timestamp = ntohl(header->timestamp);
    if (header->payload_type == rtp_ctrl_timing_sync &&
        bytes_transferred == sizeof(rtp_control_sync_packet_t)) {
      control_sync_packet((rtp_control_sync_packet_t *)header);
    } else if (header->payload_type == rtp_ctrl_retransmit_reply &&
               bytes_transferred ==
                   sizeof(rtp_control_retransmit_reply_packet_t)) {
      control_retransmit_packet(
          (rtp_control_retransmit_reply_packet_t *)header);
    } else {
      LOGE() << "Unknown RTP control packet, type: " << header->payload_type
             << " size: " << bytes_transferred;
    }
  }
}

void ap_audio_stream_service::control_sync_packet(
    rtp_control_sync_packet_t *packet) {
  LOGI() << "audio CONTROL SYNC packet";
}

void ap_audio_stream_service::control_retransmit_packet(
    rtp_control_retransmit_reply_packet_t *packet) {
  LOGI() << "audio CONTROL RETRANSMIT packet";
}

void ap_audio_stream_service::on_thread_start() {
  if (handler_) {
    handler_->on_thread_start();
  }
}

void ap_audio_stream_service::on_thread_stop() {
  if (handler_) {
    handler_->on_thread_stop();
  }
}

void ap_audio_stream_service::cache_packet(const uint16_t seq,
                                           const uint8_t *buf,
                                           std::size_t length) {
  cached_packet_ptr pk = std::make_shared<cached_packet_t>();
  pk->sequence = seq;
  pk->data.assign(buf, buf + length);
  cached_queue_.push(pk);
}

void ap_audio_stream_service::process_cached_packet(bool flush /* = false*/) {
  while (!cached_queue_.empty()) {
    auto p = cached_queue_.top();
    if (flush || p->sequence == expected_seq_) {
      cached_queue_.pop();
      rtp_audio_data_packet_t *header =
          (rtp_audio_data_packet_t *)p->data.data();
      audio_data_packet(header, p->data.size());
      expected_seq_ = p->sequence + 1;
    } else {
      break;
    }
  }
}

} // namespace service
} // namespace aps
