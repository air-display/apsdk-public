#include "ap_video_stream_service.h"
#include <crypto/ap_crypto.h>
#include <utils/logger.h>

namespace aps {
namespace service {
ap_video_stream_session::ap_video_stream_session(
    asio::io_context &io_ctx, aps::ap_crypto_ptr &crypto,
    aps::ap_handler_ptr handler /*= 0*/)
    : aps::network::tcp_session_base(io_ctx), handler_(handler),
      crypto_(crypto) {
  LOGD() << "ap_video_stream_session(" << std::hex << this
         << ") is allocating.";
}

ap_video_stream_session::~ap_video_stream_session() {
  LOGD() << "ap_video_stream_session(" << std::hex << this
         << ") is destroying.";
}

void ap_video_stream_session::start() { post_receive_packet_header(); }

void ap_video_stream_session::post_receive_packet_header() {
  memset(&(packet_.header), 0, sizeof(packet_header_t));
  asio::async_read(
      socket_, asio::buffer(&(packet_.header), packet_header_t::LENGTH),
      asio::bind_executor(
          strand_,
          std::bind(&ap_video_stream_session::on_packet_header_received,
                    shared_from_this(), // Keep the session alive
                    std::placeholders::_1, std::placeholders::_2)));
}

void ap_video_stream_session::on_packet_header_received(
    const asio::error_code &e, std::size_t bytes_transferred) {
  if (!e) {
    post_receive_packet_payload();
    return;
  } else {
    handle_socket_error(e);
  }
}

void ap_video_stream_session::post_receive_packet_payload() {
  packet_.payload.resize(packet_.header.payload_size, 0);
  asio::async_read(
      socket_,
      asio::buffer(packet_.payload.data(), packet_.header.payload_size),
      asio::bind_executor(
          strand_,
          std::bind(&ap_video_stream_session::on_packet_payload_received,
                    shared_from_this(), // Keep the session alive
                    std::placeholders::_1, std::placeholders::_2)));
}

void ap_video_stream_session::on_packet_payload_received(
    const asio::error_code &e, std::size_t bytes_transferred) {
  if (!e) {
    LOGV() << "mirror stream payload received, size: " << bytes_transferred;

    process_packet();

    post_receive_packet_header();
  } else {
    handle_socket_error(e);
  }
}

void ap_video_stream_session::process_packet() {
  if (mirror_payload_video == packet_.header.payload_type) {
    // Process the video packet
    LOGV() << "mirror VIDEO packet: " << packet_.payload.size();
    crypto_->decrypt_video_frame(packet_.payload.data(),
                                 packet_.payload.size());
  } else if (mirror_payload_codec == packet_.header.payload_type) {
    // Process the codec packet
    LOGV() << "mirror CODEC packet: " << packet_.payload.size();
  } else if (mirror_payload_5 == packet_.header.payload_type) {
    // Process the 5 packet
    LOGV() << "mirror 5 packet: " << packet_.payload.size();
  } else if (mirror_payload_4096 == packet_.header.payload_type) {
    // Process the 4096 packet
    LOGV() << "mirror 4096 packet: " << packet_.payload.size();
  } else {
    // Unknown packet
    LOGE() << "Unknown payload type: " << packet_.header.payload_type;
  }
}

void ap_video_stream_session::handle_socket_error(const asio::error_code &e) {
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

ap_video_stream_service::ap_video_stream_service(aps::ap_crypto_ptr &crypto,
                                                 uint16_t port,
                                                 aps::ap_handler_ptr &handler)
    : aps::network::tcp_service_base("ap_video_stream_service", port, true),
      handler_(handler), crypto_(crypto) {}

ap_video_stream_service::~ap_video_stream_service() {}

aps::network::tcp_session_ptr ap_video_stream_service::prepare_new_session() {
  return std::make_shared<ap_video_stream_session>(io_context(), crypto_,
                                                   handler_);
}
} // namespace service
} // namespace aps
