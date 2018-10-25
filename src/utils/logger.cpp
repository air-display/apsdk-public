#include "logger.h"

log_config logger::log_config_;

void logger::init_logger(bool header, log_level level)
{
    log_config_.headers = header;
    log_config_.level = level;
}

logger::logger() : opened(false)
{
}

logger::logger(log_level level) : opened(false)
{
    msglevel = level;
    if (log_config_.headers) {
        operator<<("[" + get_lable(level) + "]");
    }
}

logger::~logger()
{
    if (opened) {
        std::cout << std::endl;
    }
    opened = false;
}


