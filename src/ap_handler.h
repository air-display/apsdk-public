#ifndef AP_HANDLER_H
#define AP_HANDLER_H
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <ap_session.h>
#include <ap_types.h>

namespace aps {
/// <summary>
///
/// </summary>
class ap_handler {
public:
  /// <summary>
  ///
  /// </summary>
  virtual ~ap_handler(){};

  /// <summary>
  ///
  /// </summary>
  virtual void on_thread_start(){};

  /// <summary>
  ///
  /// </summary>
  virtual void on_thread_stop(){};

  /// <summary>
  ///
  /// </summary>
  /// <param name="session"></param>
  virtual void on_session_begin(aps::ap_session_ptr session) = 0;

  /// <summary>
  ///
  /// </summary>
  virtual void on_session_end(const uint64_t session_id) = 0;
};

/// <summary>
///
/// </summary>
typedef std::shared_ptr<ap_handler> ap_handler_ptr;
} // namespace aps
#endif // AP_HANDLER_H
