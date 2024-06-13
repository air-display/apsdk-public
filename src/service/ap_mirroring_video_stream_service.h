/* 
 *  File: ap_mirroring_video_stream_service.h
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

#include <ap_handler.h>
#include <ap_session.h>
#include <crypto/ap_crypto.h>
#include <network/tcp_service.h>
#include <service/ap_mirroring_video_stream_service_details.h>
#include <utils/packing.h>

using namespace aps::service::mirror::details;

namespace aps {
namespace service {
class ap_mirroring_video_stream_connection : public network::tcp_connection_base,
                                             public std::enable_shared_from_this<ap_mirroring_video_stream_connection> {
public:
  ap_mirroring_video_stream_connection(asio::io_context &io_ctx, ap_crypto_ptr &crypto,
                                       ap_mirroring_session_handler_ptr handler = 0);

  ~ap_mirroring_video_stream_connection();

  virtual void start() override;

protected:
  void post_receive_packet_header();

  void on_packet_header_received(const asio::error_code &e, std::size_t bytes_transferred);

  void post_receive_packet_payload();

  void on_packet_payload_received(const asio::error_code &e, std::size_t bytes_transferred);

  void process_packet();

  void handle_socket_error(const asio::error_code &e);

private:
  ap_mirroring_session_handler_ptr handler_;

  ap_crypto_ptr crypto_;

  std::vector<uint8_t> buffer_;

  sms_packet_header_t *header_;

  uint8_t *payload_;
};

class ap_mirroring_video_stream_service : public network::tcp_service_base {
public:
  explicit ap_mirroring_video_stream_service(ap_crypto_ptr &crypto, uint16_t port,
                                             ap_mirroring_session_handler_ptr &handler);

  ~ap_mirroring_video_stream_service();

protected:
  virtual network::tcp_connection_ptr prepare_new_connection() override;

private:
  ap_mirroring_session_handler_ptr handler_;

  ap_crypto_ptr crypto_;
};

typedef std::shared_ptr<ap_mirroring_video_stream_service> ap_mirroring_video_stream_service_ptr;
} // namespace service
} // namespace aps
