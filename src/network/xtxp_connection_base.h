/* 
 *  File: xtxp_connection_base.h
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
#include <array>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include <asio.hpp>

#include <network/tcp_service.h>
#include <network/xtxp_message.h>

namespace aps {
namespace network {
/// <summary>
///
/// </summary>
typedef std::function<void(const request &req, response &res)> request_handler;

/// <summary>
/// path -> handler
/// </summary>
typedef std::map<std::string, request_handler> path_map;

/// <summary>
/// method -> path table
/// </summary>
typedef std::map<std::string, path_map> method_map;

/// <summary>
///
/// </summary>
typedef std::map<std::string, method_map> scheme_map;

struct request_route_s {
  std::string scheme;
  std::string method;
  std::string path;
  request_handler handler;
};
typedef request_route_s request_route_t;

class request_route_table {
  /// <summary>
  ///
  /// </summary>
  typedef scheme_map route_table;

public:
  enum error_code {
    BAD_VALUE,
    UNKNOWN_SCHEME,
    UNKNOWN_METHOD,
    UNKNOWN_PATH,
  };

  request_route_table();
  ~request_route_table();

  void register_request_route(const request_route_t &route);

  request_handler query_handler(const request &req, error_code &ec);

private:
  route_table route_table_;
  std::mutex mtx_;
};

class xtxp_connection_base : public tcp_connection_base {
public:
  explicit xtxp_connection_base(asio::io_context &io_ctx);

  ~xtxp_connection_base();

  void register_request_route(const request_route_t &route);

  virtual void start() override;

  void send_request(const request &req);

  bool is_reversed();

  void reverse();

  virtual void add_common_header(const request &req, response &res);

  virtual void method_not_found_handler(const request &req, response &res);

  virtual void path_not_found_handler(const request &req, response &res);

  virtual void post_receive_message_head();

  virtual void on_message_head_received(const asio::error_code &e, std::size_t bytes_transferred);

  virtual void post_receive_message_content();

  virtual void on_message_content_received(const asio::error_code &e, std::size_t bytes_transferred);

  virtual void post_send_response(const response &res);

  virtual void on_response_sent(const asio::error_code &e, std::size_t bytes_transferred);

  virtual void handle_socket_error(const asio::error_code &e);

  std::size_t body_completion_condition(const asio::error_code &error, std::size_t bytes_transferred);

  virtual void process_request();

  virtual void process_response();

protected:
  virtual std::shared_ptr<xtxp_connection_base> shared_from_self() = 0;

private:
  request request_;
  response response_;
  bool is_reversed_;
  asio::streambuf in_stream_;
  asio::streambuf out_stream_;
  http_message_parser parser_;
  request_route_table route_table_;
};

typedef std::shared_ptr<xtxp_connection_base> xtxp_connection_base_ptr;
typedef std::weak_ptr<xtxp_connection_base> xtxp_connection_base_weak_ptr;
} // namespace network
} // namespace aps
