#pragma once
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include <asio.hpp>

#include <ap_config.h>

namespace aps {
namespace service {
class ap_media_data_store {
  /// <summary>
  ///
  /// </summary>
  static constexpr const char *MLHLS_SCHEME = "mlhls://";

  /// <summary>
  ///
  /// </summary>
  static constexpr const char *NFHLS_SCHEME = "nfhls://";

  /// <summary>
  ///
  /// </summary>
  static constexpr const char *SCHEME_LIST = "mlhls://|nfhls://";

  /// <summary>
  ///
  /// </summary>
  static constexpr const char *HOST_LIST = "localhost|127\\.0\\.0\\.1";

  /// <summary>
  ///
  /// </summary>
  static constexpr const char *MLHLS_HOST = "mlhls://localhost";

  /// <summary>
  ///
  /// </summary>
  static constexpr const char *MASTER_M3U8 = "master.m3u8";

  /// <summary>
  ///
  /// </summary>
  static constexpr const char *INDEX_M3U8 = "index.m3u8";

  /// <summary>
  ///
  /// </summary>
  static constexpr const char *HTTP_SCHEME = "http://";

  /// <summary>
  ///
  /// </summary>
  typedef std::map<std::string, std::string> media_data;

public:
  static ap_media_data_store &get();

  void set_store_root(uint16_t port);

  bool request_media_data(const std::string &primary_uri,
                          const std::string &session_id);

  std::string process_media_data(const std::string &uri,
                                 const std::string &data);

  std::string query_media_data(const std::string &path);

  void reset();

protected:
  void add_media_data(const std::string &uri, const std::string &data);

  bool is_local_m3u8_uri(const std::string &uri);

  bool is_primary_data_uri(const std::string &uri);

  void send_fcup_request(const std::string &uri);

  std::string adjust_primary_media_data(const std::string &data);

  std::string adjust_secondary_meida_data(const std::string &data);

  ap_media_data_store();

  ~ap_media_data_store();

private:
  uint32_t request_id_;
  std::string session_id_;
  std::string primary_uri_;
  std::stack<std::string> uri_stack_;

  std::string host_;
  media_data media_data_;
  std::mutex mtx_;
};
} // namespace service
} // namespace aps
