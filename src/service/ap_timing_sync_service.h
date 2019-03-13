#pragma once
#include <memory>
#include <vector>

#include <asio.hpp>

#include <ap_types.h>
#include <crypto/ap_crypto.h>
#include <network/udp_service.h>
#include <utils/packing.h>

namespace aps {
namespace service {
class ap_timing_sync_service : public network::udp_service_base {
public:
  ap_timing_sync_service(const uint16_t port = 0);

  ~ap_timing_sync_service();

  void set_server_endpoint(const asio::ip::address &addr, uint16_t port);

  void post_send_query();

protected:
  virtual void on_send_to(asio::ip::udp::endpoint remote_endpoint,
                          const asio::error_code &e,
                          std::size_t bytes_transferred) override;

  void post_recv_reply();

  virtual void on_recv_from(asio::ip::udp::endpoint &remote_endpoint,
                            const asio::error_code &e,
                            std::size_t bytes_transferred) override;

private:
  asio::ip::udp::endpoint remote_endpoint_;

  rtp_timming_sync_packet_t query_packet_;

  rtp_timming_sync_packet_t reply_packet_;
};

typedef std::shared_ptr<ap_timing_sync_service> ap_timing_sync_service_ptr;
} // namespace service
} // namespace aps
