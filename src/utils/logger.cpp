#if defined(ANDROID)
#include <android/log.h>
#endif
#include "logger.h"

log_config logger::log_config_;

void logger::init_logger(bool header, log_level level) {
  log_config_.headers = header;
  log_config_.level = level;
}

logger::logger() : opened_(false) {}

logger::logger(log_level level) : opened_(false) {
  msglevel_ = level;
  if (log_config_.headers) {
    operator<<("[" + get_lable(level) + "]");
  }
}

logger::~logger() {
  if (opened_) {
    oss_ << std::endl;
    std::string msg = oss_.str();
#if defined(ANDROID)
    __android_log_write(msglevel_, "APS", msg.c_str());
#else
    std::cout << msg;
#endif
  }
  opened_ = false;
}
