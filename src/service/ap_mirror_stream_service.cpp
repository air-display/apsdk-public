#include <ap_types.h>
#include <crypto/ap_crypto.h>
#include <service/ap_mirror_stream_service.h>
#include <utils/logger.h>
#include <sstream>

namespace aps {
namespace service {
ap_mirror_stream_connection::ap_mirror_stream_connection(
    asio::io_context &io_ctx, ap_crypto_ptr &crypto,
    ap_mirror_session_handler_ptr handler /*= 0*/)
    : network::tcp_connection_base(io_ctx),
      handler_(handler),
      buffer_(SMS_BUFFER_SIZE),
      crypto_(crypto) {
  header_ = (sms_packet_header_t *)buffer_.data();
  payload_ = buffer_.data() + sizeof(sms_packet_header_t);

  LOGD() << "ap_video_stream_session(" << std::hex << this
         << ") is allocating.";
}

ap_mirror_stream_connection::~ap_mirror_stream_connection() {
  LOGD() << "ap_video_stream_session(" << std::hex << this
         << ") is destroying.";
#if defined(WIN32) && PERSIST_VIDEO_DATA_TO_FILE
  close_video_data_file();
#endif
}

void ap_mirror_stream_connection::start() { post_receive_packet_header(); }

void ap_mirror_stream_connection::post_receive_packet_header() {
  memset(header_, 0, sizeof(sms_packet_header_t));
  asio::async_read(
      socket_, asio::buffer(header_, sizeof(sms_packet_header_t)),
      asio::bind_executor(
          strand_,
          std::bind(&ap_mirror_stream_connection::on_packet_header_received,
                    shared_from_this(),  // Keep the session alive
                    std::placeholders::_1, std::placeholders::_2)));
}

void ap_mirror_stream_connection::on_packet_header_received(
    const asio::error_code &e, std::size_t bytes_transferred) {
  if (e) {
    handle_socket_error(e);
    return;
  }

  // Receive payload
  post_receive_packet_payload();
}

void ap_mirror_stream_connection::post_receive_packet_payload() {
  asio::async_read(
      socket_, asio::buffer(payload_, header_->payload_size),
      asio::bind_executor(
          strand_,
          std::bind(&ap_mirror_stream_connection::on_packet_payload_received,
                    shared_from_this(),  // Keep the session alive
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
  // Convert the stream to
  // 00 00 00 01 SPS  00 00 00 01 PPS  00 00 00 01 NALU
  // 00 00 00 01 NALU 00 00 00 01 NALU 00 00 00 01 ...

  if (sms_video_data == header_->payload_type ||
      sms_payload_4096 == header_->payload_type) {
    // Process the video packet
    LOGV() << "mirror VIDEO packet: " << header_->payload_size;
    // Parse the packet, each packet contains 1 NALU,
    // replace the first 4bytes with 00 00 00 01
    sms_video_data_packet_t *p = (sms_video_data_packet_t *)header_;
    crypto_->decrypt_video_frame(payload_, p->payload_size);
    
#if defined(WIN32) && PERSIST_VIDEO_DATA_TO_FILE
    append_nalu(p);
#endif

    if (handler_) {
      handler_->on_mirror_stream_data(p);
    }
  } else if (sms_video_codec == header_->payload_type) {
    // Process the codec packet
    // Here we need to construct two NALUs for SPS and PPS
    // 00 00 00 01 SPS  00 00 00 01 PPS
    LOGV() << "mirror CODEC packet: " << header_->payload_size;
    sms_video_codec_packet_t *p = (sms_video_codec_packet_t *)header_;

#if defined(WIN32) && PERSIST_VIDEO_DATA_TO_FILE
    init_video_data_file(p);
#endif

    if (handler_) {
      handler_->on_mirror_stream_codec(p);
    }
  } else if (sms_payload_5 == header_->payload_type) {
    // Process the 5 packet
    LOGV() << "mirror 5 packet: " << header_->payload_size;
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

#if defined(WIN32) && defined(PERSIST_VIDEO_DATA_TO_FILE)
void ap_mirror_stream_connection::init_video_data_file(
    sms_video_codec_packet_t *p) {
  if (video_data_file_.is_open()) {
    video_data_file_.close();
  }

  auto mode =
      std::ios_base::binary | std::ios_base::binary | std::ios_base::trunc;
  video_data_file_.open("ap-mirroring.h264", mode);

  uint32_t sc = htonl(0x01);
  std::ostringstream oss;

  // Parse SPS
  uint8_t *cursor = (uint8_t *)p->start;
  for (int i = 0; i < p->sps_count; i++) {
    oss.write((char *)&sc, 0x4);
    uint16_t sps_length = *(uint16_t *)cursor;
    sps_length = ntohs(sps_length);
    cursor += sizeof(uint16_t);
    oss.write((char *)cursor, sps_length);
    cursor += sps_length;
  }

  // Parse PPS
  uint8_t pps_count = *cursor++;
  for (int i = 0; i < pps_count; i++) {
    oss.write((char *)&sc, 0x4);
    uint16_t pps_length = *(uint16_t *)cursor;
    pps_length = ntohs(pps_length);
    cursor += sizeof(uint16_t);
    oss.write((char *)cursor, pps_length);
    cursor += pps_length;
  }
  std::string buffer = oss.str();
  video_data_file_.write((char *)buffer.c_str(), buffer.length());
}

void ap_mirror_stream_connection::append_nalu(sms_video_data_packet_t *p) {
  p->payload[0] = 0;
  p->payload[1] = 0;
  p->payload[2] = 0;
  p->payload[3] = 1;
  video_data_file_.write((char *)p->payload, p->payload_size);
}

void ap_mirror_stream_connection::close_video_data_file() {
  video_data_file_.close();
}

#endif

ap_mirror_stream_service::ap_mirror_stream_service(
    ap_crypto_ptr &crypto, uint16_t port,
    ap_mirror_session_handler_ptr &handler)
    : network::tcp_service_base("ap_video_stream_service", port, true),
      handler_(handler),
      crypto_(crypto) {
  bind_thread_actions(
      std::bind(&ap_mirror_stream_service::on_thread_start, this),
      std::bind(&ap_mirror_stream_service::on_thread_stop, this));
}

ap_mirror_stream_service::~ap_mirror_stream_service() {}

network::tcp_connection_ptr
ap_mirror_stream_service::prepare_new_connection() {
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

}  // namespace service
}  // namespace aps
