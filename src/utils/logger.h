#ifndef LOGGER_H
#define LOGGER_H
#pragma once

#include <iostream>
#include <sstream>
#include <string>

#ifdef NDEBUG // Release
#define DEFAULT_LOG_LEVEL LL_INFO
#else // Debug
#define DEFAULT_LOG_LEVEL LL_DEBUG
#endif

typedef enum log_level_e {
  LL_UNKNOWN = 0,
  LL_DEFAULT,
  LL_VERBOSE,
  LL_DEBUG,
  LL_INFO,
  LL_WARN,
  LL_ERROR,
  LL_FATAL,
  LL_SILENT
} log_level;

typedef struct log_config_s {
  bool headers;
  log_level level;

  log_config_s() {
#if defined(ANDROID)
    headers = false;
    level = LL_DEFAULT;
#else
    headers = true;
    level = DEFAULT_LOG_LEVEL;
#endif
  }
} log_config;

class logger {
public:
  static void init_logger(bool header, log_level level);

  logger();
  logger(log_level level);
  ~logger();

  template <class T> logger &operator<<(const T &msg) {
    if (msglevel_ >= log_config_.level) {
      oss_ << msg;
      opened_ = true;
    }
    return *this;
  }

protected:
  inline std::string get_lable(log_level level) {
    std::string label;
    switch (level) {
    case LL_DEFAULT:
      label = "DEFAULT";
      break;
    case LL_VERBOSE:
      label = "VERBOSE";
      break;
    case LL_DEBUG:
      label = "DEBUG";
      break;
    case LL_INFO:
      label = "INFO";
      break;
    case LL_WARN:
      label = "WARN";
      break;
    case LL_ERROR:
      label = "ERROR";
      break;
    case LL_FATAL:
      label = "FATAL";
      break;
    case LL_SILENT:
      label = "SILENT";
      break;
    default:
      label = "";
      break;
    }
    return label;
  }

private:
  bool opened_;
  log_level msglevel_;
  std::ostringstream oss_;

  static log_config log_config_;
};

#define LOG() logger(log_level::LL_DEFAULT)
#define LOGV() logger(log_level::LL_VERBOSE)
#define LOGD() logger(log_level::LL_DEBUG)
#define LOGI() logger(log_level::LL_INFO)
#define LOGW() logger(log_level::LL_WARN)
#define LOGE() logger(log_level::LL_ERROR)
#define LOGF() logger(log_level::LL_FATAL)

#endif // LOGGER_H
