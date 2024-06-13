/* 
 *  File: ap_mirroring_timing_sync_service.h
 *  Project: apsdk
 *  Created: Oct 25, 2018
 *  Author: Sheen Tian
 *  
 *  This file is part of apsdk (https://github.com/air-display/apsdk-public) 
 *  Copyright (C) 2018-2024 Sheen Tian 
 *  
 *  apsdk is free software: you can redistribute it and/or modify it under the terms 
 *  of the GNU General Public License as published by the Free Software Foundation, 
 *  either version 3 of the License, or (at your option) any later version.
 *  
 *  apsdk is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *  See the GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License along with Foobar. 
 *  If not, see <https://www.gnu.org/licenses/>.
 */

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
class ap_mirroring_timing_sync_service : public network::udp_service_base {
public:
  ap_mirroring_timing_sync_service(const uint16_t port = 0);

  ~ap_mirroring_timing_sync_service();

  void set_server_endpoint(const asio::ip::address &addr, uint16_t port);

  void post_send_query();

protected:
  virtual void on_send_to(asio::ip::udp::endpoint remote_endpoint, const asio::error_code &e,
                          std::size_t bytes_transferred) override;

  void post_recv_reply();

  virtual void on_recv_from(asio::ip::udp::endpoint &remote_endpoint, const asio::error_code &e,
                            std::size_t bytes_transferred) override;

private:
  asio::ip::udp::endpoint remote_endpoint_;

  rtp_timming_sync_packet_t query_packet_;

  rtp_timming_sync_packet_t reply_packet_;
};

typedef std::shared_ptr<ap_mirroring_timing_sync_service> ap_mirroring_timing_sync_service_ptr;
} // namespace service
} // namespace aps
