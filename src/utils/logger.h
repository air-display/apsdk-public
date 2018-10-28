#ifndef LOGGER_H
#define LOGGER_H
#pragma once

#include <string>
#include <iostream>

typedef enum log_level_e {
    LL_DEBUG,
    LL_INFO,
    LL_WARN,
    LL_ERROR
} log_level;

typedef struct log_config_s {
    bool headers = true;
    log_level level = LL_DEBUG;
} log_config;

class logger {
public:
    static void init_logger(bool header, log_level level);

    logger();
    logger(log_level level);
    ~logger();

    template<class T>
    logger& operator << (const T& msg) {
        std::ios_base::fmtflags f(std::cout.flags());
        if (msglevel >= log_config_.level) {
            std::cout << msg;
            opened = true;
        }
        std::cout.flags(f);
        return *this;
    }

protected:
    inline std::string get_lable(log_level level)
    {
        std::string label;
        switch (level) {
        case LL_DEBUG: label = "DEBUG"; break;
        case LL_INFO:  label = "INFO "; break;
        case LL_WARN:  label = "WARN "; break;
        case LL_ERROR: label = "ERROR"; break;
        }
        return label;
    }

private:
    log_level msglevel;
    bool opened;

    static log_config log_config_;
};

#define LOGD() logger(log_level::LL_DEBUG)
#define LOGI() logger(log_level::LL_INFO)
#define LOGW() logger(log_level::LL_WARN)
#define LOGE() logger(log_level::LL_ERROR)

#endif  // LOGGER_H
