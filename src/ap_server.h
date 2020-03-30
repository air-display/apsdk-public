#ifndef AP_SERVER_H
#define AP_SERVER_H
#pragma once

#include <memory>
#if __ANDROID__
#include <jni.h>
#endif

#include <ap_config.h>
#include <ap_export.h>
#include <ap_handler.h>

namespace aps {
/// <summary>
///
/// </summary>
class APS_EXPORT ap_server {
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

#if __ANDROID__
  /// <summary>
  ///
  /// </summary>
  /// <param name="vm"></param>
  static void setJavaVM(JavaVM *vm);
#endif

private:
  /// <summary>
  ///
  /// </summary>
  class implementation;

  /// <summary>
  ///
  /// </summary>
  implementation *impl_;
};

typedef std::shared_ptr<ap_server> ap_server_ptr;
} // namespace aps
#endif // AP_SERVER_H
