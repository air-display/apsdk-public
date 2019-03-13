#include <sstream>

#include <ap_types.h>
#include <crypto/ap_crypto.h>
#include <service/ap_mirror_stream_service.h>
#include <utils/logger.h>
#include <utils/utils.h>

namespace aps {
namespace service {
ap_mirror_stream_connection::ap_mirror_stream_connection(
    asio::io_context &io_ctx, ap_crypto_ptr &crypto,
    ap_mirror_session_handler_ptr handler /*= 0*/)
    : network::tcp_connection_base(io_ctx), handler_(handler),
      buffer_(SMS_BUFFER_SIZE), crypto_(crypto) {
  header_ = (sms_packet_header_t *)buffer_.data();
  payload_ = buffer_.data() + sizeof(sms_packet_header_t);

  LOGD() << "ap_video_stream_session(" << std::hex << this
         << ") is allocating.";
}

ap_mirror_stream_connection::~ap_mirror_stream_connection() {
  LOGD() << "ap_video_stream_session(" << std::hex << this
         << ") is destroying.";
}

void ap_mirror_stream_connection::start() { post_receive_packet_header(); }

void ap_mirror_stream_connection::post_receive_packet_header() {
  memset(header_, 0, sizeof(sms_packet_header_t));
  asio::async_read(
      socket_, asio::buffer(header_, sizeof(sms_packet_header_t)),
      asio::bind_executor(
          strand_,
          std::bind(&ap_mirror_stream_connection::on_packet_header_received,
                    shared_from_this(), // Keep the session alive
                    std::placeholders::_1, std::placeholders::_2)));
}

void ap_mirror_stream_connection::on_packet_header_received(
    const asio::error_code &e, std::size_t bytes_transferred) {
  if (e) {
    handle_socket_error(e);
    return;
  }

  // Because all fields are little-endian we need to convert them to network
  // byte order first and then convert them from network byte order to local
  // host byte order
  header_->payload_size = swap_bytes(header_->payload_size);
  header_->payload_size = ntohl(header_->payload_size);

  header_->payload_type = swap_bytes(header_->payload_type);
  header_->payload_type = ntohs(header_->payload_type);

  header_->timestamp = swap_bytes(header_->timestamp);
  header_->timestamp = ntohll(header_->timestamp);

  // Receive payload
  post_receive_packet_payload();
}

void ap_mirror_stream_connection::post_receive_packet_payload() {
  asio::async_read(
      socket_, asio::buffer(payload_, header_->payload_size),
      asio::bind_executor(
          strand_,
          std::bind(&ap_mirror_stream_connection::on_packet_payload_received,
                    shared_from_this(), // Keep the session alive
                    std::placeholders::_1, std::placeholders::_2)));
}

void ap_mirror_stream_connection::on_packet_payload_received(
    const asio::error_code &e, std::size_t bytes_transferred) {
  if (!e) {
    LOGV() << "mirror stream payload received, size: " << bytes_transferred;

    process_packet();

    post_receive_packet_header();
  } else {
    handle_socket_error(e);
  }
}

void ap_mirror_stream_connection::process_packet() {
  if (sms_video_data == header_->payload_type ||
      sms_payload_4096 == header_->payload_type) {
    // Process the video packet
    LOGV() << "mirror VIDEO packet: " << header_->payload_size;
    sms_video_data_packet_t *p = (sms_video_data_packet_t *)header_;
    crypto_->decrypt_video_frame(payload_, p->payload_size);
    if (handler_) {
      handler_->on_mirror_stream_data(p);
    }
  } else if (sms_video_codec == header_->payload_type) {
    // Process the codec packet
    LOGV() << "mirror CODEC packet: " << header_->payload_size;
    sms_video_codec_packet_t *p = (sms_video_codec_packet_t *)header_;
    if (handler_) {
      handler_->on_mirror_stream_codec(p);
    }
  } else if (sms_payload_5 == header_->payload_type) {
    // Process the 5 packet
    LOGV() << "mirror 5 packet: " << header_->payload_size;
    if (handler_) {
      handler_->on_mirror_stream_heartbeat();
    }
  } else {
    // Unknown packet
    LOGE() << "Unknown payload type: " << header_->payload_type;
  }
}

void ap_mirror_stream_connection::handle_socket_error(
    const asio::error_code &e) {
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

ap_mirror_stream_service::ap_mirror_stream_service(
    ap_crypto_ptr &crypto, uint16_t port,
    ap_mirror_session_handler_ptr &handler)
    : network::tcp_service_base("ap_video_stream_service", port, true),
      handler_(handler), crypto_(crypto) {
  bind_thread_actions(
      std::bind(&ap_mirror_stream_service::on_thread_start, this),
      std::bind(&ap_mirror_stream_service::on_thread_stop, this));
}

ap_mirror_stream_service::~ap_mirror_stream_service() {}

network::tcp_connection_ptr ap_mirror_stream_service::prepare_new_connection() {
  return std::make_shared<ap_mirror_stream_connection>(io_context(), crypto_,
                                                       handler_);
}

void ap_mirror_stream_service::on_thread_start() {
  if (handler_) {
    handler_->on_thread_start();
  }
}

void ap_mirror_stream_service::on_thread_stop() {
  if (handler_) {
    handler_->on_thread_stop();
  }
}

} // namespace service
} // namespace aps
