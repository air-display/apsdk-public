#include <chrono>
#include <cstring>
#include <ctime>
#include <regex>
#include <time.h>

#include <hlsparser/hlsparse.h>
#include <utils/utils.h>

using namespace std::chrono;

const uint32_t EPOCH = 2208988800ULL;        // January 1970, in NTP seconds.
const double NTP_SCALE_FRAC = 4294967296ULL; // NTP fractional unit.

uint64_t get_ntp_timestamp() {
  uint64_t seconds = 0;
  uint64_t fraction = 0;

  milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

  seconds = ms.count() / 1000 + EPOCH;
  fraction = (uint64_t)((ms.count() % 1000) * NTP_SCALE_FRAC) / 1000;

  return (seconds << 32) | fraction;
}

uint64_t normalize_ntp_to_ms(uint64_t ntp) {
  uint64_t milliseconds = (ntp >> 32) * 1000;
  uint32_t fraction = (uint32_t)((ntp & 0x0ffffffff) * 1000.f / NTP_SCALE_FRAC);
  return (milliseconds + fraction);
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

std::string generate_mac_address() {
  uint64_t ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

  static char buffer[32];
  memset(buffer, 0, 32);
  std::sprintf(buffer,
               "%02X:%02X:%02X:%02X:%02X:%02X",
               (uint8_t)((ts >> 0) & 0xff),
               (uint8_t)((ts >> 8) & 0xff),
               (uint8_t)((ts >> 16) & 0xff),
               (uint8_t)((ts >> 24) & 0xff),
               (uint8_t)((ts >> 32) & 0xff),
               (uint8_t)((ts >> 40) & 0xff));
  return buffer;
}

std::string string_replace(const std::string &str, const std::string &pattern, const std::string &with) {
  std::regex p(pattern);
  return std::regex_replace(str, p, with);
}

std::string generate_file_name() {
  time_t now = time(0);
  std::tm *local_now = localtime(&now);
  std::ostringstream oss;
  // clang-format off
  oss << local_now->tm_year + 1900 << "-"
    << local_now->tm_mon + 1 << "-"
    << local_now->tm_mday << "-"
    << local_now->tm_hour << "-"
    << local_now->tm_min << "-"
    << local_now->tm_sec;
  // clang-format on
  return oss.str();
}

int compare_string_no_case(const char *str1, const char *str2) {
#if defined(WIN32) || defined(MS_VER_)
  return _strcmpi(str1, str2);
#else
  return strcasecmp(str1, str2);
#endif
}

bool get_youtube_url(const char *data, uint32_t length, std::string &url) {
  static std::regex pattern("#YT-EXT-CONDENSED-URL:BASE-URI=\"(.*)\",PARAMS=");
  std::cmatch groups;

  if (std::regex_search(data, groups, pattern)) {
    if (groups.size() > 1) {
      url = groups.str(1);
      return true;
    }
  }

  return false;
}

std::string get_best_quality_stream_uri(const char *data, uint32_t length) {
  HLSCode r = hlsparse_global_init();
  master_t master_playlist;
  r = hlsparse_master_init(&master_playlist);
  r = hlsparse_master(data, length, &master_playlist);
  stream_inf_list_t *best_quality_stream = 0;
  stream_inf_list_t *stream_inf = &master_playlist.stream_infs;
  return master_playlist.media.data->uri;
  while (stream_inf && stream_inf->data) {
    if (!best_quality_stream) {
      best_quality_stream = stream_inf;
    } else if (stream_inf->data->bandwidth > best_quality_stream->data->bandwidth) {
      best_quality_stream = stream_inf;
    }
    stream_inf = stream_inf->next;
  }
  if (best_quality_stream) {
    return best_quality_stream->data->uri;
  }

  return std::string();
}
