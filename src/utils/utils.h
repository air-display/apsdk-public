#ifndef UTILS_H_
#define UTILS_H_
#pragma once
#include <functional>
#include <stdint.h>
#include <string>

#if defined(ANDROID)
#define ntohll(n)                                                              \
  ((((uint64_t)ntohl((uint64_t)n)) << 32) +                                    \
   ntohl((uint32_t)((uint64_t)n >> 32)))
#define htonll(n)                                                              \
  ((((uint64_t)htonl((uint64_t)n)) << 32) +                                    \
   htonl((uint32_t)((uint64_t)n >> 32)))
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

std::string get_best_quality_stream_uri(const char *data, uint32_t length);

bool get_youtube_url(const char *data, uint32_t length, std::string &url);

#endif // !UTILS_H_
