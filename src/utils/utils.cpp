#include "utils.h"
#include <chrono>
#include <ctime>
#include <string.h>
#include <time.h>


using namespace std::chrono;

uint64_t get_ntp_timestamp() {
  const uint32_t EPOCH = 2208988800ULL;        // January 1970, in NTP seconds.
  const double NTP_SCALE_FRAC = 4294967296ULL; // NTP fractional unit.
  uint64_t seconds = 0;
  uint64_t fraction = 0;

  milliseconds ms =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch());

  seconds = ms.count() / 1000 + EPOCH;
  fraction = ((ms.count() % 1000) * NTP_SCALE_FRAC) / 1000;

  return (seconds << 32) | fraction;
}

const char *gmt_time_string() {
  static char date_buf[64];
  memset(date_buf, 0, 64);

  std::time_t now = std::time(0);
  if (std::strftime(date_buf, 64, "%c GMT", std::gmtime(&now)))
    return date_buf;
  else
    return 0;
}

// std::string get_value_of_key(const std::string& s)
//{
//
//}
