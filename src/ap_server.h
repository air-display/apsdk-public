#ifndef AP_SERVER_H
#define AP_SERVER_H
#pragma once

#include <ap_config.h>
#include <ap_handler.h>
#include <memory>

namespace aps {
class ap_server {
public:
  ap_server(const aps::ap_config &config);
  ~ap_server();

  void set_handler(ap_handler_ptr hanlder);

  bool start();

  void stop();

private:
  class implementation;
  std::unique_ptr<implementation> impl_;
};
} // namespace aps
#endif // AP_SERVER_H
