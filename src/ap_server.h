#ifndef AP_SERVER_H
#define AP_SERVER_H
#pragma once

#include <memory>

#include <ap_config.h>
#include <ap_handler.h>

namespace aps {
/// <summary>
///
/// </summary>
class ap_server {
public:
  /// <summary>
  ///
  /// </summary>
  explicit ap_server();

  /// <summary>
  ///
  /// </summary>
  ~ap_server();

  /// <summary>
  ///
  /// </summary>
  /// <param name="config"></param>
  void set_config(ap_config_ptr &config);

  /// <summary>
  ///
  /// </summary>
  /// <param name="hanlder"></param>
  void set_handler(ap_handler_ptr &hanlder);

  /// <summary>
  ///
  /// </summary>
  /// <returns></returns>
  bool start();

  /// <summary>
  ///
  /// </summary>
  void stop();

  /// <summary>
  ///
  /// </summary>
  uint16_t get_service_port();

private:
  /// <summary>
  ///
  /// </summary>
  class implementation;

  /// <summary>
  ///
  /// </summary>
  std::unique_ptr<implementation> impl_;
};

typedef std::shared_ptr<ap_server> ap_server_ptr;
} // namespace aps
#endif // AP_SERVER_H
