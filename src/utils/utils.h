#ifndef UTILS_H_
#define UTILS_H_
#pragma once
#include <stdint.h>
#include <string>
#include <functional>

#if defined(ANDROID)
#define ntohll(n) \
  ((((uint64_t)ntohl((uint32_t)n)) << 32) + ntohl((uint32_t)(n >> 32)))
#define htonll(n) \
  ((((uint64_t)htonl((uint32_t)n)) << 32) + htonl((uint32_t)(n >> 32)))
#endif

/// <summary>
///
/// </summary>
uint64_t get_ntp_timestamp();

/// <summary>
///
/// </summary>
const char *gmt_time_string();

/// <summary>
/// Sets the name for the thread.
/// </summary>
/// <param name="t">For windows platofmr this is the thread handle.</param>
/// <param name="name">The thread name.</param>
void set_thread_name(void *t, const char *name);

/// <summary>
///
/// </summary>
/// <param name="name"></param>
void set_current_thread_name(const char *name);

/// <summary>
///
/// </summary>
typedef std::function<void()> thread_actoin;

/// <summary>
/// 
/// </summary>
struct thread_guard_s {
  thread_guard_s(thread_actoin start, thread_actoin stop)
      : start_(start), stop_(stop) {
    if (start_) start_();
  }

  ~thread_guard_s() {
    if (stop_) stop_();
  }

 public:
  thread_actoin start_;
  thread_actoin stop_;
};
typedef thread_guard_s thread_guard_t;

#endif  // !UTILS_H_
