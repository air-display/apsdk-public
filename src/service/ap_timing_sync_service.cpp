#include <service/ap_timing_sync_service.h>
#include <utils/utils.h>

using namespace std::chrono;
using namespace aps::network;

namespace aps {
namespace service {
ap_timing_sync_service::ap_timing_sync_service(const uint16_t port /*= 0*/)
    : network::udp_service_base("ap_timing_sync_service", port) {}

ap_timing_sync_service::~ap_timing_sync_service() {}

void ap_timing_sync_service::set_server_endpoint(const asio::ip::address &addr,
                                                 uint16_t port) {
  remote_endpoint_.address(addr);
  remote_endpoint_.port(port);
}

void ap_timing_sync_service::post_send_query() {
  query_packet_.marker = 1;
  query_packet_.payload_type = rtp_timing_query;
  query_packet_.original_timestamp = htonll(0);
  query_packet_.receive_timestamp = htonll(0);
  query_packet_.transmit_timestamp = htonll(get_ntp_timestamp());

  post_send_to((uint8_t *)&query_packet_, sizeof(query_packet_),
               remote_endpoint_);
}

void ap_timing_sync_service::on_send_to(asio::ip::udp::endpoint remote_endpoint,
                                        const asio::error_code &e,
                                        std::size_t bytes_transferred) {
  if (e)
    LOGE() << "Failed to send timing query: " << e.message();
  else {
    LOGD() << "Timing query packet sent successfully";
    post_recv_reply();
  }
}

void ap_timing_sync_service::post_recv_reply() {
  post_recv_from((uint8_t *)&reply_packet_, sizeof(reply_packet_),
                 remote_endpoint_);
}

void ap_timing_sync_service::on_recv_from(
    asio::ip::udp::endpoint &remote_endpoint, const asio::error_code &e,
    std::size_t bytes_transferred) {
  if (e)
    LOGE() << "Failed to receive timing query: " << e.message();
  else {
    reply_packet_.sequence = ntohs(reply_packet_.sequence);
    reply_packet_.timestamp = ntohl(reply_packet_.timestamp);
    reply_packet_.original_timestamp = ntohll(reply_packet_.original_timestamp);
    reply_packet_.receive_timestamp = ntohll(reply_packet_.receive_timestamp);
    reply_packet_.transmit_timestamp = ntohll(reply_packet_.transmit_timestamp);
    LOGD() << "Timing reply packet received successfully";
  }
}
} // namespace service
} // namespace aps
