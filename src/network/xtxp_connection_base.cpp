#include <network/xtxp_connection_base.h>

namespace aps {
namespace network {

request_route_table::request_route_table() {}

request_route_table::~request_route_table() {}

void request_route_table::register_request_route(const request_route_t &route) {
  std::lock_guard<std::mutex> l(mtx_);
  auto it_scheme_method = route_table_.find(route.scheme);
  if (it_scheme_method == route_table_.end()) {
    path_map path_to_handler;
    path_to_handler[route.path] = route.handler;

    method_map method_to_path;
    method_to_path[route.method] = path_to_handler;

    route_table_[route.scheme] = method_to_path;
  } else {
    method_map &method_path = it_scheme_method->second;

    auto it_path_handler = method_path.find(route.method);
    if (it_path_handler == method_path.end()) {
      path_map path_to_handler;
      path_to_handler[route.path] = route.handler;

      method_path[route.method] = path_to_handler;
    } else {
      path_map &path_handler = it_path_handler->second;
      path_handler[route.path] = route.handler;
    }
  }
}

request_hanlder request_route_table::query_handler(const request &req,
                                                   error_code &ec) {
  std::lock_guard<std::mutex> l(mtx_);

  std::string scheme;
  auto index = req.scheme_version.find('/');
  if (std::string::npos != index) {
    scheme = req.scheme_version.substr(0, index);
  }

  request_hanlder handler;

  auto it_scheme_method = route_table_.find(scheme);
  if (it_scheme_method == route_table_.end()) {
    // Scheme not found
    ec = UNKNOWN_SCHEME;
  } else {
    // Scheme found
    method_map &method_path = it_scheme_method->second;

    auto it_path_handler = method_path.find(req.method);
    if (it_path_handler == method_path.end()) {
      // Method not found
      ec = UNKNOWN_METHOD;
    } else {
      // Method found
      path_map &path_handler = it_path_handler->second;

      std::string path = req.uri;
      auto index = req.uri.find('?');
      if (std::string::npos != index)
        path = req.uri.substr(0, index);

      auto it_handler = path_handler.find(path);
      if (it_handler != path_handler.end()) {
        handler = it_handler->second;
        return handler;
      }

      it_handler = path_handler.find("*");
      if (it_handler != path_handler.end()) {
        handler = it_handler->second;
        return handler;
      }

      // Path not found
      ec = UNKNOWN_PATH;
    }
  }

  return handler;
}

xtxp_connection_base::xtxp_connection_base(asio::io_context &io_ctx)
    : tcp_connection_base(io_ctx), is_reversed_(false) {}

xtxp_connection_base::~xtxp_connection_base() {}

void xtxp_connection_base::register_request_route(
    const request_route_t &route) {
  route_table_.register_request_route(route);
}

void xtxp_connection_base::start() { post_receive_message_head(); }

void xtxp_connection_base::send_request(const request &req) {
  std::string data = req.serialize();
  socket_.send(asio::buffer(data.data(), data.length()));
}

bool xtxp_connection_base::is_reversed() { return is_reversed_; }

void xtxp_connection_base::reverse() { is_reversed_ = true; }

void xtxp_connection_base::add_common_header(const request &req,
                                             response &res) {}

void xtxp_connection_base::post_receive_message_head() {
  asio::async_read_until(
      socket_, in_stream_, RNRN_LINE_BREAK,
      asio::bind_executor(
          strand_, std::bind(&xtxp_connection_base::on_message_head_received,
                             shared_from_self(), std::placeholders::_1,
                             std::placeholders::_2)));
}

void xtxp_connection_base::method_not_found_handler(const request &req,
                                                    response &res) {
  LOGE() << "***** Method Not Allowed " << request_.method << " "
         << request_.uri;

  // Method not found
  // res.with_status(method_not_allowed);
  res.with_status(ok);
}

void xtxp_connection_base::path_not_found_handler(const request &req,
                                                  response &res) {
  LOGE() << "***** Path Not Found " << request_.method << " " << request_.uri;

  // Path not found
  // res.with_status(not_found);
  res.with_status(ok);
}

void xtxp_connection_base::on_message_head_received(
    const asio::error_code &e, std::size_t bytes_transferred) {
  // If error then return
  if (e) {
    handle_socket_error(e);
    return;
  }

  std::string head_data(asio::buffers_begin(in_stream_.data()),
                        asio::buffers_begin(in_stream_.data()) +
                            bytes_transferred);
  in_stream_.consume(bytes_transferred);

  bool parse_result = false;
  if (is_reversed_) {
    // Parse the request head
    parse_result = parser_.parse(response_, head_data);
  } else {
    // Parse the request head
    parse_result = parser_.parse(request_, head_data);
  }
  if (!parse_result) {
    // Invalid request head, close this session
    LOGE() << "Invalid message (neither request nor response)";
    stop();
    return;
  }

  int content_length =
      is_reversed_ ? response_.content_length : request_.content_length;

  if (content_length == 0) {
    // This is a head only message, process and prepare to read next one
    if (is_reversed_) {
      process_response();
    } else {
      process_request();
    }
    post_receive_message_head();
    return;
  }

  // The message has content data to be read
  if (content_length - in_stream_.size()) {
    post_receive_message_content();
    return;
  }

  if (is_reversed_) {
    // No more data to be read just build the message
    if (in_stream_.size()) {
      response_.content.clear();
      response_.content.resize(response_.content_length, 0);

      response_.content.assign(asio::buffers_begin(in_stream_.data()),
                               asio::buffers_begin(in_stream_.data()) +
                                   in_stream_.size());
    }
  } else {
    // No more data to be read just build the message
    if (in_stream_.size()) {
      request_.content.clear();
      request_.content.resize(request_.content_length, 0);

      request_.content.assign(asio::buffers_begin(in_stream_.data()),
                              asio::buffers_begin(in_stream_.data()) +
                                  in_stream_.size());
    }
  }
  in_stream_.consume(in_stream_.size());

  if (is_reversed_) {
    process_response();
  } else {
    process_request();
  }
  // Prepare to read next message
  post_receive_message_head();
}

void xtxp_connection_base::post_receive_message_content() {
  if (is_reversed_) {
    response_.content.clear();
  } else {
    request_.content.clear();
  }

  asio::async_read(
      socket_, in_stream_,
      std::bind(&xtxp_connection_base::body_completion_condition,
                shared_from_self(), std::placeholders::_1,
                std::placeholders::_2),
      asio::bind_executor(
          strand_, std::bind(&xtxp_connection_base::on_message_content_received,
                             shared_from_self(), std::placeholders::_1,
                             std::placeholders::_2)));
}

void xtxp_connection_base::on_message_content_received(
    const asio::error_code &e, std::size_t bytes_transferred) {
  if (e) {
    handle_socket_error(e);
    return;
  }

  if (is_reversed_) {
    response_.content.assign(asio::buffers_begin(in_stream_.data()),
                             asio::buffers_begin(in_stream_.data()) +
                                 in_stream_.size());
    in_stream_.consume(in_stream_.size());
  } else {
    request_.content.assign(asio::buffers_begin(in_stream_.data()),
                            asio::buffers_begin(in_stream_.data()) +
                                in_stream_.size());
    in_stream_.consume(in_stream_.size());
  }

  if (is_reversed_) {
    process_response();
  } else {
    process_request();
  }
  post_receive_message_head();
}

void xtxp_connection_base::post_send_response(const response &res) {
  std::ostream os(&out_stream_);
  os << res.serialize();
  asio::async_write(
      socket_, out_stream_,
      asio::bind_executor(strand_,
                          std::bind(&xtxp_connection_base::on_response_sent,
                                    shared_from_self(), std::placeholders::_1,
                                    std::placeholders::_2)));
}

void xtxp_connection_base::on_response_sent(const asio::error_code &e,
                                            std::size_t bytes_transferred) {
  if (e) {
    return handle_socket_error(e);
  }

  // LOGV() << ">>>>> " << bytes_transferred << " bytes sent successfully";
}

void xtxp_connection_base::handle_socket_error(const asio::error_code &e) {
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

std::size_t
xtxp_connection_base::body_completion_condition(const asio::error_code &error,
                                                std::size_t bytes_transferred) {
  return request_.content_length - in_stream_.size();
}

void xtxp_connection_base::process_request() {
  response res(request_.scheme_version);

  request_route_table::error_code ec;
  auto handler = route_table_.query_handler(request_, ec);

  if (!handler) {
    if (request_route_table::UNKNOWN_PATH == ec) {
      path_not_found_handler(request_, res);
    } else if (request_route_table::UNKNOWN_METHOD == ec) {
      method_not_found_handler(request_, res);
    } else {
      LOGE() << "Unknown request: " << request_.scheme_version;
      return;
    }
  } else {
    handler(request_, res);
  }

  add_common_header(request_, res);

  post_send_response(res);
}

void xtxp_connection_base::process_response() {}

} // namespace network
} // namespace aps
