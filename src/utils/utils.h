#ifndef UTILS_H_
#define UTILS_H_
#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <thread>

#include <asio.hpp>

/// <summary>
///
/// </summary>
typedef asio::thread aps_thread;

#ifdef __GNUC__
#ifdef ANDROID
#include <endian.h>
#define ntohll(n) ntohq(n)
#define htonll(n) htonq(n)
#else
#define ntohll(n)                                                              \
  (((uint64_t)ntohl((uint32_t)n)) << 32) + ntohl(((uint64_t)n) >> 32)
#define htonll(n)                                                              \
  (((uint64_t)htonl((uint32_t)n)) << 32) + htonl(((uint64_t)n) >> 32)
#endif // ANDROID
#endif // __GNUC__

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

inline uint16_t swap_bytes(const uint16_t v) {
  return ((v & 0x00ff) << 8) | ((v & 0xff00) >> 8);
}

inline uint32_t swap_bytes(const uint32_t v) {
  uint32_t r = 0;
  uint8_t *pr = (uint8_t *)&r;
  uint8_t *pv = (uint8_t *)&v;
  pr[0] = pv[3];
  pr[1] = pv[2];
  pr[2] = pv[1];
  pr[3] = pv[0];
  return r;
}

inline uint64_t swap_bytes(const uint64_t v) {
  uint64_t r = 0;
  uint8_t *pr = (uint8_t *)&r;
  uint8_t *pv = (uint8_t *)&v;
  pr[0] = pv[7];
  pr[1] = pv[6];
  pr[2] = pv[5];
  pr[3] = pv[4];
  pr[4] = pv[3];
  pr[5] = pv[2];
  pr[6] = pv[1];
  pr[7] = pv[0];
  return r;
}

/// <summary>
///
/// </summary>
uint64_t get_ntp_timestamp();

/// <summary>
///
/// </summary>
uint64_t normalize_ntp_to_ms(uint64_t ntp);

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

std::string generate_file_name();

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
    if (start_)
      start_();
  }

  ~thread_guard_s() {
    if (stop_)
      stop_();
  }

public:
  thread_actoin start_;
  thread_actoin stop_;
};
typedef thread_guard_s thread_guard_t;

int compare_string_no_case(const char *str1, const char *str2);

bool get_youtube_url(const char *data, uint32_t length, std::string &url);

#endif // !UTILS_H_
