#pragma once
#include "utils.h"
#include <ctime>

const char* gmt_time_string()
{
    static char date_buf[64];
    memset(date_buf, 0, 64);

    std::time_t now = std::time(0);
    if (std::strftime(date_buf, 64, "%c GMT", std::gmtime(&now)))
        return date_buf;
    else
        return 0;
}
