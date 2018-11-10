#ifndef UTILS_H_
#define UTILS_H_
#pragma once
#include <stdint.h>
#include <string>

#if defined(ANDROID)
#define ntohll(n) ( (((uint64_t)ntohl((uint32_t)n)) << 32) + ntohl((uint32_t)(n >> 32)) )
#define htonll(n) ( (((uint64_t)htonl((uint32_t)n)) << 32) + htonl((uint32_t)(n >> 32)) )
#endif

/// <summary>
/// 
/// </summary>
uint64_t get_ntp_timestamp();

/// <summary>
/// 
/// </summary>
const char* gmt_time_string();

/// <summary>
/// Sets the name for the thread.
/// </summary>
/// <param name="t">For windows platofmr this is the thread handle.</param>
/// <param name="name">The thread name.</param>
void set_thread_name(void* t, const char* name);


void set_current_thread_name(const char* name);


std::string get_value_of_key(const std::string& s);

#endif // !UTILS_H_

