#ifndef UTILS_H_
#define UTILS_H_
#pragma once
#include <stdint.h>

#if defined(ANDROID)
#define ntohll(n) ( (((uint64_t)ntohl((uint32_t)n)) << 32) + ntohl((uint32_t)(n >> 32)) )
#define htonll(n) ( (((uint64_t)htonl((uint32_t)n)) << 32) + htonl((uint32_t)(n >> 32)) )
#endif

const char* gmt_time_string();

uint64_t get_ntp_timestamp();

#endif // !UTILS_H_

