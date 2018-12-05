#pragma once
#include <network/xtxp_connection_base.h>

namespace aps {
namespace network {
xtxp_connection_base::xtxp_connection_base(asio::io_context &io_ctx)
    : tcp_connection_base(io_ctx), is_reversed_(false) {}

xtxp_connection_base::~xtxp_connection_base() {}

void xtxp_connection_base::register_rtsp_request_handler(
    request_hanlder handler, const std::string &method,
    const std::string &path /*= std::string()*/) {
  auto path_map = rtsp_request_handlers_.find(method);
  if (path_map == rtsp_request_handlers_.end()) {
    path_handler_map path_map;
    path_map[path] = handler;
    rtsp_request_handlers_[method] = path_map;
  } else {
    path_map->second[path] = handler;
  }
}

void xtxp_connection_base::register_http_request_handler(
    request_hanlder handler, const std::string &method,
    const std::string &path /*= std::string()*/) {
  auto path_map = http_request_handlers_.find(method);
  if (path_map == http_request_handlers_.end()) {
    path_handler_map path_map;
    path_map[path] = handler;
    http_request_handlers_[method] = path_map;
  } else {
    path_map->second[path] = handler;
  }
}

void xtxp_connection_base::register_request_handler(
    service_type_t service, request_hanlder handler, const std::string &method,
    const std::string &path /*= std::string()*/) {
  if (RTSP == service) {
    register_rtsp_request_handler(handler, method, path);
  } else if (HTTP == service) {
    register_http_request_handler(handler, method, path);
  } else
    return;
}

void xtxp_connection_base::register_request_route(
    const request_route_t &route) {
  register_request_handler(route.service, route.handler, route.method,
                           route.path);
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
                             shared_from_this(), std::placeholders::_1,
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

  std::string head_data(
      asio::buffers_begin(in_stream_.data()),
      asio::buffers_begin(in_stream_.data()) + bytes_transferred);
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

      response_.content.assign(
          asio::buffers_begin(in_stream_.data()),
          asio::buffers_begin(in_stream_.data()) + in_stream_.size());
    }
  } else {
    // No more data to be read just build the message
    if (in_stream_.size()) {
      request_.content.clear();
      request_.content.resize(request_.content_length, 0);

      request_.content.assign(
          asio::buffers_begin(in_stream_.data()),
          asio::buffers_begin(in_stream_.data()) + in_stream_.size());
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
                shared_from_this(), std::placeholders::_1,
                std::placeholders::_2),
      asio::bind_executor(
          strand_, std::bind(&xtxp_connection_base::on_message_content_received,
                             shared_from_this(), std::placeholders::_1,
                             std::placeholders::_2)));
}

void xtxp_connection_base::on_message_content_received(
    const asio::error_code &e, std::size_t bytes_transferred) {
  if (e) {
    handle_socket_error(e);
    return;
  }

  if (is_reversed_) {
    response_.content.assign(
        asio::buffers_begin(in_stream_.data()),
        asio::buffers_begin(in_stream_.data()) + in_stream_.size());
    in_stream_.consume(in_stream_.size());
  } else {
    request_.content.assign(
        asio::buffers_begin(in_stream_.data()),
        asio::buffers_begin(in_stream_.data()) + in_stream_.size());
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
      asio::bind_executor(
          strand_,
          std::bind(&xtxp_connection_base::on_response_sent, shared_from_this(),
                    std::placeholders::_1, std::placeholders::_2)));
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

std::size_t xtxp_connection_base::body_completion_condition(
    const asio::error_code &error, std::size_t bytes_transferred) {
  return request_.content_length - in_stream_.size();
}

void xtxp_connection_base::process_request() {
  response res(request_.scheme_version);

  request_handler_map *handler_map = 0;
  if (0 == request_.scheme_version.find("RTSP"))
    handler_map = &rtsp_request_handlers_;
  else if (0 == request_.scheme_version.find("HTTP"))
    handler_map = &http_request_handlers_;
  else
    return;

  auto method_to_path = handler_map->find(request_.method);
  if (method_to_path == handler_map->end())
    method_not_found_handler(request_, res);
  else {
    auto path_to_handler = method_to_path->second.find("*");
    if (path_to_handler == method_to_path->second.end()) {
      std::string path = request_.uri;
      auto index = request_.uri.find('?');
      if (std::string::npos != index) path = request_.uri.substr(0, index);

      path_to_handler = method_to_path->second.find(path);
    }

    if (path_to_handler == method_to_path->second.end())
      path_not_found_handler(request_, res);
    else
      path_to_handler->second(request_, res);
  }

  res.with_header(HEADER_DATE, gmt_time_string());

  add_common_header(request_, res);

  post_send_response(res);
}

void xtxp_connection_base::process_response() {}

}  // namespace network
}  // namespace aps
