/* 
 *  File: tcp_service.h
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
#include <thread>

#include <asio.hpp>

#include <utils/logger.h>
#include <utils/utils.h>

namespace aps {
namespace network {
class tcp_connection {
public:
  virtual ~tcp_connection(){};

  virtual void start() = 0;

  virtual void stop() = 0;

  virtual asio::ip::tcp::socket &socket() = 0;
};

typedef std::shared_ptr<tcp_connection> tcp_connection_ptr;

class tcp_service {
public:
  virtual ~tcp_service(){};

  virtual const uint16_t port() const = 0;

  virtual bool start() = 0;

  virtual void stop() = 0;

  virtual asio::io_context &io_context() = 0;

  virtual tcp_connection_ptr prepare_new_connection() = 0;
};

typedef std::shared_ptr<tcp_service> tcp_service_ptr;
typedef std::weak_ptr<tcp_service> tcp_service_weak_ptr;

class tcp_connection_base : public tcp_connection {
public:
  explicit tcp_connection_base(asio::io_context &io_ctx) : socket_(io_ctx), strand_(io_ctx){};

  virtual ~tcp_connection_base() {}

  virtual void start() override {}

  virtual void stop() override {
    socket_.shutdown(asio::socket_base::shutdown_both);
    socket_.close();
  }

  virtual asio::ip::tcp::socket &socket() override { return socket_; }

protected:
  asio::ip::tcp::socket socket_;
  asio::io_context::strand strand_;
};

class tcp_service_base : public tcp_service {
public:
  tcp_service_base(const std::string &name, uint16_t port = 0, bool single_session = false)
      : single_session_(single_session), service_name_(name), io_context_(), io_work_(io_context_),
        acceptor_(io_context_), local_endpoint_(asio::ip::tcp::v6(), port), worker_thread_(0) {}

  ~tcp_service_base() { cleanup(); }

  virtual const uint16_t port() const override { return local_endpoint_.port(); }

  virtual bool start() override {
    // Setup the resources
    if (!setup())
      return false;

    // Post the first accept operation
    post_accept();

    return true;
  }

  virtual void stop() override { cleanup(); }

  virtual asio::io_context &io_context() override { return io_context_; }

protected:
  void post_accept() {
    // Create a new client session for incoming connection
    new_session_ = prepare_new_connection();

    // Perform accept operation on the new client session
    acceptor_.async_accept(new_session_->socket(),
                           std::bind(&tcp_service_base::on_accept, this, std::placeholders::_1));

    LOGD() << "Session (" << std::hex << new_session_.get() << ") is waiting";
  }

  void on_accept(const asio::error_code &e) {
    if (!e) {
      // Start the new session
      new_session_->start();

      LOGD() << "Session (" << std::hex << new_session_.get() << ") accepted and started";

      if (single_session_)
        return;

      // Post a new accept operation
      post_accept();
    } else
      LOGE() << "Failed to accept the new session: " << e.message();
  }

protected:
  bool setup() {
    // Create the worker thread
    worker_thread_ = create_aps_thread([&]() {
#if defined(DEBUG) || defined(_DEBUG)
      set_current_thread_name(service_name_.c_str());
#endif

      io_context_.run();

    });

    if (!worker_thread_)
      return false;

    // Create the acceptor
    acceptor_.open(local_endpoint_.protocol());
    acceptor_.set_option(asio::ip::v6_only(false));
    acceptor_.set_option(asio::socket_base::reuse_address(true));
    acceptor_.bind(local_endpoint_);
    acceptor_.listen();
    local_endpoint_ = acceptor_.local_endpoint();

    return true;
  }

  void cleanup() {
    io_context_.stop();

    if (worker_thread_) {
      worker_thread_->join();
      worker_thread_.reset();
    }
  }

private:
  bool single_session_;
  std::string service_name_;
  asio::io_context io_context_;
  asio::io_context::work io_work_;
  asio::ip::tcp::acceptor acceptor_;
  asio::ip::tcp::endpoint local_endpoint_;
  aps_thread worker_thread_;

  tcp_connection_ptr new_session_;
};

class idle_tcp_server {
public:
  idle_tcp_server(uint16_t port) {}

  ~idle_tcp_server() {}

private:
};
} // namespace network
} // namespace aps
