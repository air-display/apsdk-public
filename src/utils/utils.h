#ifndef UTILS_H_
#define UTILS_H_
#pragma once
#include <stdint.h>
#include <functional>
#include <string>
#include <asio.hpp>
#if defined(ANDROID)
#include <endian.h>
#endif

#if defined(ANDROID)
#define ntohll(n) ntohq(n)
#define htonll(n) htonq(n)
#endif

inline float _ntohf(float f) {
  union {
    float f;
    uint32_t u;
  } value;

  value.f = f;
  value.u = ntohl(value.u);
  return value.f;
}

inline float _htonf(float f) {
  union {
    float f;
    uint32_t u;
  } value;
  value.f = f;
  value.u = htonl(value.u);
  return value.f;
}

inline double _ntohd(double d) {
  union {
    double d;
    uint64_t u;
  } value;
  value.d = d;
  value.u = ntohll(value.u);
  return value.d;
}

inline double _htond(double d) {
  union {
    double d;
    uint64_t u;
  } value;
  value.d = d;
  value.u = htonll(value.u);
  return value.d;
}

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
std::string generate_mac_address();

/// <summary>
/// 
/// </summary>
std::string string_replace(const std::string &str, const std::string &pattern,
                           const std::string &with);

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

int compare_string_no_case(const char *str1, const char *str2);

bool get_youtube_url(const char *data, uint32_t length, std::string &url);

#endif  // !UTILS_H_
