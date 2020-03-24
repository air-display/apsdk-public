#include <fstream>
#include <regex>

#include <hlsparser/hlsparse.h>
#include <service/ap_event_connection_manager.h>
#include <service/ap_media_data_store.h>
#include <utils/utils.h>

#if defined(WIN32) && !defined(NDEBUG)
#include <filesystem>
void create_serssion_folder(const std::string &session) {
  std::filesystem::create_directory(session);
}

void create_resource_file(const std::string &session, const std::string &uri,
                          const std::string &data) {
  std::string fn = generate_file_name();
  fn += string_replace(uri, "/|\\\\", "-");
  std::string path = session + "/" + fn;
  std::ofstream ofs;
  ofs.open(path, std::ofstream::out | std::ofstream::app);
  ofs << data;
  ofs.close();
}
#endif

namespace aps {
namespace service {

aps::service::ap_media_data_store &ap_media_data_store::get() {
  static ap_media_data_store s_instance;
  return s_instance;
}

ap_media_data_store::ap_media_data_store() { hlsparse_global_init(); }

ap_media_data_store::~ap_media_data_store() {}

void ap_media_data_store::set_store_root(uint16_t port) {
  std::ostringstream oss;
  oss << "localhost:" << port;
  host_ = oss.str();
}

bool ap_media_data_store::request_media_data(const std::string &primary_uri,
                                             const std::string &session_id) {
  reset();

  app_id id = get_appi_id(primary_uri);

  if (id != e_app_unknown) {
#if defined(WIN32) && !defined(NDEBUG)
    create_serssion_folder(session_id);
#endif
    app_id_ = id;
    session_id_ = session_id;
    primary_uri_ = adjust_primary_uri(primary_uri);
    send_fcup_request(primary_uri);
    return true;
  }

  // Not local m3u8 uri
  return false;
}

std::string ap_media_data_store::process_media_data(const std::string &uri,
                                                    const std::string &data) {
  std::string media_data;

  if (is_primary_data_uri(uri)) {
    master_t master_playlist;
    if (HLS_OK == hlsparse_master_init(&master_playlist)) {
      if (hlsparse_master(data.c_str(), data.length(), &master_playlist)) {

        // Save all media uri
        media_list_t *media_item = &master_playlist.media;
        while (media_item && media_item->data) {
          uri_stack_.push(media_item->data->uri);
          media_item = media_item->next;
        }

        // Save all stream uri
        stream_inf_list_t *stream_item = &master_playlist.stream_infs;
        while (stream_item && stream_item->data) {
          uri_stack_.push(stream_item->data->uri);
          stream_item = stream_item->next;
        }
      }
    }

    // Adjust the primary media data and cache it
    media_data = adjust_primary_media_data(data);
  } else {
    // Adjust the secondary media data and cache it
    media_data = adjust_secondary_meida_data(data);
  }

  std::string path = extrac_uri_path(uri);

  if (!path.empty() && !media_data.empty()) {
    add_media_data(path, media_data);
  }

  if (uri_stack_.empty()) {
    // no more data
    return primary_uri_;
  }

  auto next_uri = uri_stack_.top();
  uri_stack_.pop();
  send_fcup_request(next_uri);

  return std::string();
}

std::string ap_media_data_store::query_media_data(const std::string &path) {
  std::lock_guard<std::mutex> l(mtx_);
  auto it = media_data_.find(path);
  if (it != media_data_.end()) {
    return it->second;
  }
  return std::string();
}

void ap_media_data_store::reset() {
  app_id_ = e_app_unknown;
  request_id_ = 1;
  session_id_.clear();
  primary_uri_.clear();
  uri_stack_ = std::stack<std::string>();

  media_data_.clear();
}

ap_media_data_store::app_id
ap_media_data_store::get_appi_id(const std::string &uri) {
  // Youtube
  if (0 == uri.find(MLHLS_SCHEME))
    return e_app_youtube;

  // Netflix
  if (0 == uri.find(NFHLS_SCHEME))
    return e_app_netflix;

  return e_app_unknown;
}

void ap_media_data_store::add_media_data(const std::string &uri,
                                         const std::string &data) {
  {
    std::lock_guard<std::mutex> l(mtx_);
    media_data_[uri] = data;
  }

#if defined(WIN32) && !defined(NDEBUG)
  create_resource_file(session_id_, uri, data);
#endif
}

bool ap_media_data_store::is_primary_data_uri(const std::string &uri) {
  if (strstr(uri.c_str(), MASTER_M3U8))
    return true;
  if (strstr(uri.c_str(), INDEX_M3U8))
    return true;

  return false;
}

void ap_media_data_store::send_fcup_request(const std::string &uri) {
  std::ostringstream oss;
  // clang-format off
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
           "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
           "<plist version=\"1.0\">\n"
           "\t<dict>\n"
           "\t\t<key>sessionID</key>\n"
           "\t\t<integer>1</integer>\n"
           "\t\t<key>type</key>\n"
           "\t\t<string>unhandledURLRequest</string>\n"
           "\t\t<key>request</key>\n"
           "\t\t<dict>\n"
           "\t\t\t<key>FCUP_Response_ClientInfo</key>\n"
           "\t\t\t<integer>1</integer>\n"
           "\t\t\t<key>FCUP_Response_ClientRef</key>\n"
           "\t\t\t<integer>40030004</integer>\n"
           "\t\t\t<key>FCUP_Response_RequestID</key>\n"
           "\t\t\t<integer>" << (request_id_++) << "</integer>\n"
           "\t\t\t<key>FCUP_Response_URL</key>\n"
           "\t\t\t<string>" << uri << "</string>\n"
           "\t\t\t<key>sessionID</key>\n"
           "\t\t\t<integer>1</integer>\n"
           "\t\t\t<key>FCUP_Response_Headers</key>\n"
           "\t\t\t<dict>\n"
           "\t\t\t\t<key>X-Playback-Session-Id</key>\n"
           "\t\t\t\t<string>" << session_id_ << "</string>\n"
           "\t\t\t\t<key>User-Agent</key>\n"
           "\t\t\t\t<string>AppleCoreMedia/1.0.0.11B554a (Apple TV; U; CPU OS 7_0_4 like Mac OS X; en_us)</string>\n"
           "\t\t\t</dict>\n"
           "\t\t</dict>\n"
           "\t</dict>\n"
           "</plist>";
  // clang-format on

  request fcup_request("HTTP/1.1", "POST", "/event");
  fcup_request.with_header("X-Apple-Session-ID", session_id_)
      .with_content_type(TEXT_APPLE_PLIST_XML)
      .with_content(oss.str());

  auto p = ap_event_connection_manager::get().get(session_id_);
  auto pp = p.lock();
  if (pp) {
    pp->send_request(fcup_request);
  }
}

std::string ap_media_data_store::adjust_primary_uri(const std::string &uri) {
  std::string s = uri;
  s = string_replace(s, SCHEME_LIST, HTTP_SCHEME);
  s = string_replace(s, HOST_LIST, host_);
  return s;
}

std::string ap_media_data_store::extrac_uri_path(const std::string &uri) {
  std::string s = uri;
  switch (app_id_) {
  case e_app_youtube:
    s = string_replace(s, MLHLS_SCHEME, "");
    s = string_replace(s, HOST_LIST, "");
  case e_app_netflix:
    s = string_replace(s, NFHLS_SCHEME, "");
    s = string_replace(s, HOST_LIST, "");
    if (s.at(0) != '/') {
      s = "/" + s;
    }
  default:
    break;
  }
  return s;
}

std::string
ap_media_data_store::adjust_primary_media_data(const std::string &data) {
  switch (app_id_) {
  case e_app_youtube:
    return adjust_mlhls_data(data);
  case e_app_netflix:
    return adjust_nfhls_data(data);
  default:
    break;
  }
  return data;
}

std::string
ap_media_data_store::adjust_secondary_meida_data(const std::string &data) {
  std::string result = data;

  static std::regex youtube_pattern(
      "#YT-EXT-CONDENSED-URL:BASE-URI=\"(.*)\",PARAMS=.*PREFIX=\"(.*)\"");
  std::cmatch groups;

  std::string base;
  std::string prefix;
  if (std::regex_search(result.c_str(), groups, youtube_pattern)) {
    if (groups.size() > 2) {
      base = groups.str(1);
      prefix = groups.str(2);
    }
  }

  if (!base.empty() && !prefix.empty()) {
    std::regex re("\n" + prefix);
    std::string fmt = "\n" + base + "/" + prefix;
    result = std::regex_replace(result, re, fmt);
  }

  return result;
}

std::string ap_media_data_store::adjust_mlhls_data(const std::string &data) {
  std::string s = data;
  s = string_replace(s, MLHLS_SCHEME, HTTP_SCHEME);
  s = string_replace(s, HOST_LIST, host_);
  return s;
}

std::string ap_media_data_store::adjust_nfhls_data(const std::string &data) {
  std::string s = data;
  std::string replace = HTTP_SCHEME;
  replace += host_;
  replace += "/";
  s = string_replace(s, NFHLS_SCHEME, replace);
  return s;
}

} // namespace service
} // namespace aps
