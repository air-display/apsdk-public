/* 
 *  File: ap_casting_media_http_service.cpp
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

#include <service/ap_casting_media_data_store.h>
#include <service/ap_casting_media_http_service.h>

namespace aps {
namespace service {

ap_casting_media_http_connection::ap_casting_media_http_connection(asio::io_context &io_ctx)
    : xtxp_connection_base(io_ctx) {
  initialize_request_handlers();
}

ap_casting_media_http_connection::~ap_casting_media_http_connection() = default;

void ap_casting_media_http_connection::get_handler(const request &req, response &res) {
  DUMP_REQUEST_WITH_CONNECTION(req);

  auto data = ap_casting_media_data_store::get().query_media_data(req.uri);
  if (data.empty()) {
    res.with_status(not_found);
    return;
  }

  res.with_status(ok).with_content_type(APPLICATION_MPEGURL).with_content(data);
}

void ap_casting_media_http_connection::add_common_header(const request &req, response &res) {
  res.with_header(HEADER_DATE, gmt_time_string())
      .with_header(HEADER_ALLOW_HEADER, HEADER_CONTENT_TYPE)
      .with_header(HEADER_ALLOW_ORIGIN, "*");
}

#define RH(x) std::bind(&ap_casting_media_http_connection::x, this, std::placeholders::_1, std::placeholders::_2)

void ap_casting_media_http_connection::initialize_request_handlers() {
  request_route_t routes_table[] = {
      {"HTTP", "GET", "*", RH(get_handler)},
  };
  for (const auto& route : routes_table) {
    register_request_route(route);
  }
}

std::shared_ptr<xtxp_connection_base> ap_casting_media_http_connection::shared_from_self() {
  return shared_from_this();
}

ap_casting_media_http_service::ap_casting_media_http_service(ap_config_ptr &config, uint16_t port /*= 0*/)
    : tcp_service_base("ap_casting_media_http_service", port), config_(config) {}

ap_casting_media_http_service::~ap_casting_media_http_service() = default;

network::tcp_connection_ptr ap_casting_media_http_service::prepare_new_connection() {
  return std::make_shared<ap_casting_media_http_connection>(io_context());
}

} // namespace service
} // namespace aps
