#pragma once
#include <array>
#include <asio.hpp>
#include <map>
#include <string>
#include <utils/logger.h>
#include <utils/packing.h>
#include <vector>


//#define DUMP_REQUEST_BODY 1

namespace aps {
namespace service {
namespace details {
/// <summary>
///
/// </summary>
enum stream_type_e {
  audio = 96,
  video = 110,
};
typedef stream_type_e stream_type_t;

/// <summary>
///
/// </summary>
PACKED(struct pair_verify_header_s {
  uint8_t is_first_frame;
  uint8_t reserved0;
  uint8_t reserved1;
  uint8_t reserved2;
});
typedef pair_verify_header_s pair_verify_header_t;

// RTSP line break and delimiter
static const char *RN_LINE_BREAK = "\r\n";
static const char *RNRN_LINE_BREAK = "\r\n\r\n";
static const char *CHAR_BLANK = " ";
static const char *CHAR_COLON = ":";

// RTSP header keys
static const char *HEADER_CONTENT_TYPE = "Content-Type";
static const char *HEADER_CONTENT_LENGTH = "Content-Length";
static const char *HEADER_CSEQ = "CSeq";
static const char *HEADER_SERVER = "Server";
static const char *HEADER_SESSION = "Session";
static const char *HEADER_DATE = "Date";
static const char *HEADER_AUDIO_JACK_STATUS = "Audio-Jack-Status";

// RTSP content type
static const char *APPLICATION_OCTET_STREAM = "application/octet-stream";
static const char *APPLICATION_BINARY_PLIST =
    "application/x-apple-binary-plist";
static const char *APPLICATION_DMAP_TAGGED = "application/x-dmap-tagged";
static const char *TEXT_APPLE_PLIST_XML = "text/x-apple-plist+xml";
static const char *TEXT_PARAMETERS = "text/parameters";
static const char *IMAGE_JPEG = "image/jpeg";
static const char *IMAGE_PNG = "image/png";

static const char *ERROR_STATUS_RESPONSE =
    // clang-format off
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
    "<plist version=\"1.0\">\n"
    "<dict>\n"
    "\t<key>errorCode</key>\n"
    "\t<integer>0</integer>\n"
    "</dict>\n"
    "</plist>";
// clang-format on

static const char *MLHLS_SCHEME = "mlhls://localhost/master.m3u8";
static const char *NFHLS_SCHEME = "nfhls://";

typedef std::map<std::string, std::string> header_map;

/// <summary>
///
/// </summary>
enum status_type_e {
  switching_protocols = 101,
  ok = 200,
  bad_request = 400,
  not_found = 404,
  method_not_allowed = 405,
  internal_error = 500,
};
typedef status_type_e status_type_t;

static std::map<int, const std::string> g_status_code_sting_map = {
    {switching_protocols, "Switching Protocols"},
    {ok, "OK"},
    {bad_request, "Bad Request"},
    {not_found, "Not Found"},
    {method_not_allowed, "Method Not Allowed"},
    {internal_error, "Internal Error"},
};

/// <summary>
///
/// </summary>
class request {
public:
  std::string method;
  std::string uri;
  std::string scheme_version;
  std::string cseq;
  int content_length;
  std::string content_type;
  header_map headers;
  std::vector<uint8_t> body;

  request() {}

  request(const std::string &scheme_ver, const std::string &methot,
          const std::string &uri)
      : scheme_version(scheme_ver), method(methot), uri(uri),
        content_length(0) {}

  request &with_content_type(const std::string &type) {
    content_type = type;
    return *this;
  }

  request &with_content(const std::vector<uint8_t> data) {
    body = data;
    content_length = data.size();
    return *this;
  }

  request &with_content(const uint8_t *data, int length) {
    std::copy(data, data + length, std::back_inserter(body));
    content_length = length;
    return *this;
  }

  request &with_content(const std::string &data) {
    std::copy(data.begin(), data.end(), std::back_inserter(body));
    content_length = data.length();
    return *this;
  }

  request &with_header(const std::string &name, const std::string &value) {
    headers[name] = value;
    return *this;
  }

  std::string format_to_string() const {
    std::ostringstream oss;
    oss << method << CHAR_BLANK << uri << CHAR_BLANK << scheme_version
        << RN_LINE_BREAK;

    for (auto &kv : headers)
      oss << kv.first << CHAR_COLON << CHAR_BLANK << kv.second << RN_LINE_BREAK;

    oss << HEADER_CONTENT_LENGTH << CHAR_COLON << CHAR_BLANK << content_length
        << RN_LINE_BREAK;

    if (!content_type.empty())
      oss << HEADER_CONTENT_TYPE << CHAR_COLON << CHAR_BLANK << content_type
          << RN_LINE_BREAK;

    oss << RN_LINE_BREAK;

    std::copy(body.begin(), body.end(), std::ostream_iterator<uint8_t>(oss));

    return oss.str();
  }

  void dump(const std::string tag) const {
    std::ostringstream oss;
    oss << tag << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< " << std::endl
        << "    Request: " << method << " " << uri << " " << scheme_version
        << std::endl
        << "    Header:" << std::endl;
    for (auto &header : headers)
      oss << "        " << header.first << ": " << header.second << std::endl;

#ifdef DUMP_REQUEST_BODY
    oss << "    Content:";
    if (content_length)
      oss.write((char *)body.data(), body.size());
    else
      oss << "<EMPTY>";
#endif
    LOGD() << oss.str();
  }
};

/// <summary>
///
/// </summary>
class response {
public:
  std::string scheme_version;
  status_type_t status_code;
  std::string status_text;
  header_map headers;
  int content_length;
  std::string content_type;
  std::vector<uint8_t> content;

  explicit response(const std::string &scheme_ver)
      : scheme_version(scheme_ver), status_code(status_type_t::ok),
        status_text("OK"), content_length(0), no_common_headers_(false) {}

  response &set_no_common_headers(bool b) {
    no_common_headers_ = b;
    return *this;
  }

  bool no_common_headers() const { return no_common_headers_; }

  response &with_status(status_type_t code) {
    status_code = code;
    auto code_string = g_status_code_sting_map.find(code);
    if (code_string != g_status_code_sting_map.end()) {
      status_text = code_string->second;
    } else {
      status_text = "No Description";
    }

    return *this;
  }

  response &with_content_type(const std::string &type) {
    content_type = type;
    return *this;
  }

  response &with_content(const std::vector<uint8_t> data) {
    content = data;
    content_length = data.size();
    return *this;
  }

  response &with_content(const uint8_t *data, int length) {
    std::copy(data, data + length, std::back_inserter(content));
    content_length = length;
    return *this;
  }

  response &with_content(const std::string &data) {
    std::copy(data.begin(), data.end(), std::back_inserter(content));
    content_length = data.length();
    return *this;
  }

  response &with_header(const std::string &name, const std::string &value) {
    headers[name] = value;
    return *this;
  }

  std::string format_to_string() const {
    std::ostringstream oss;
    oss << scheme_version << CHAR_BLANK << status_code << CHAR_BLANK
        << status_text << RN_LINE_BREAK;

    for (auto &kv : headers)
      oss << kv.first << CHAR_COLON << CHAR_BLANK << kv.second << RN_LINE_BREAK;

    oss << HEADER_CONTENT_LENGTH << CHAR_COLON << CHAR_BLANK << content_length
        << RN_LINE_BREAK;

    if (!content_type.empty())
      oss << HEADER_CONTENT_TYPE << CHAR_COLON << CHAR_BLANK << content_type
          << RN_LINE_BREAK;

    oss << RN_LINE_BREAK;

    std::copy(content.begin(), content.end(),
              std::ostream_iterator<uint8_t>(oss));

    return oss.str();
  }

  void format_to_stream(asio::streambuf &buf) const {
    std::ostream os(&buf);

    os << scheme_version << CHAR_BLANK << status_code << CHAR_BLANK
       << status_text << RN_LINE_BREAK;

    for (auto &kv : headers)
      os << kv.first << CHAR_COLON << CHAR_BLANK << kv.second << RN_LINE_BREAK;

    os << HEADER_CONTENT_LENGTH << CHAR_COLON << CHAR_BLANK << content_length
       << RN_LINE_BREAK;

    if (!content_type.empty())
      os << HEADER_CONTENT_TYPE << CHAR_COLON << CHAR_BLANK << content_type
         << RN_LINE_BREAK;

    os << RN_LINE_BREAK;

    std::copy(content.begin(), content.end(),
              std::ostream_iterator<uint8_t>(os));
  }

private:
  bool no_common_headers_;
};

/// <summary>
///
/// </summary>
class request_parser {
  /// The current state of the parser.
  enum state {
    token_start,
    token_method,
    token_uri,
    token_scheme_version,
    token_expecting_new_line,
    token_header_line_start,
    token_header_lws,
    token_header_name,
    token_space_before_header_value,
    token_header_value,
    token_expecting_last_line
  };

public:
  enum parse_result {
    parse_more,
    parse_done,
    parse_fail,
  };

  /// Construct ready to parse the request method.
  request_parser() : state_(token_start) {}

  /// Reset to initial parser state.
  void reset() { state_ = token_start; }

  bool parse(request &req, std::string data) {
    req.method.clear();
    req.scheme_version.clear();
    req.uri.clear();
    req.cseq.clear();
    req.content_length = 0;
    req.headers.clear();
    req.body.clear();

    int i = 0;
    while (i != data.length()) {
      parse_result result = consume(req, data[i++]);
      if (parse_more != result)
        reset();

      if (parse_fail == result)
        return false;
    }
    return true;
  }

private:
  /// Handle the next character of input.
  parse_result consume(request &req, char input) {
    switch (state_) {
    case token_start:
      if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return parse_fail;
      } else {
        state_ = token_method;
        req.method.push_back(input);
        return parse_more;
      }
    case token_method:
      if (input == ' ') {
        state_ = token_uri;
        return parse_more;
      } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return parse_fail;
      } else {
        req.method.push_back(input);
        return parse_more;
      }
    case token_uri:
      if (input == ' ') {
        state_ = token_scheme_version;
        return parse_more;
      } else if (!is_char(input) || is_ctl(input)) {
        return parse_fail;
      } else {
        req.uri.push_back(input);
        return parse_more;
      }
    case token_scheme_version:
      if (input == '\r') {
        state_ = token_expecting_new_line;
        return parse_more;
      } else if (!is_char(input)) {
        return parse_fail;
      } else {
        req.scheme_version.push_back(input);
        return parse_more;
      }
    case token_expecting_new_line:
      if (input == '\n') {
        state_ = token_header_line_start;
        return parse_more;
      } else {
        return parse_fail;
      }
    case token_header_line_start:
      if (input == '\r') {
        // There is no more headers
        state_ = token_expecting_last_line;
        return parse_more;
      }
      // else if (!value.empty() && (input == ' ' || input == '\t'))
      //{
      //    state_ = token_header_lws;
      //    return parse_more;
      //}
      else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return parse_fail;
      } else {
        name.clear();
        value.clear();
        name.push_back(input);
        state_ = token_header_name;
        return parse_more;
      }
    // case token_header_lws:
    //    if (input == '\r')
    //    {
    //        state_ = token_expecting_new_line;
    //        return parse_more;
    //    }
    //    else if (input == ' ' || input == '\t')
    //    {
    //        return parse_more;
    //    }
    //    else if (!is_char(input) || is_ctl(input))
    //    {
    //        return parse_fail;
    //    }
    //    else
    //    {
    //        state_ = token_header_value;
    //        name.push_back(input);
    //        return parse_more;
    //    }
    case token_header_name:
      if (input == ':') {
        state_ = token_space_before_header_value;
        return parse_more;
      } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return parse_fail;
      } else {
        name.push_back(input);
        return parse_more;
      }
    case token_space_before_header_value:
      if (input == ' ') {
        state_ = token_header_value;
        return parse_more;
      } else {
        return parse_fail;
      }
    case token_header_value:
      if (input == '\r') {
        if (0 == std::strcmp(HEADER_CONTENT_LENGTH, name.c_str())) {
          char *end = 0;
          req.content_length = std::strtol(value.c_str(), &end, 10);
        }
        if (0 == std::strcmp(HEADER_CONTENT_TYPE, name.c_str())) {
          req.content_type = value;
        }
        if (0 == std::strcmp(HEADER_CSEQ, name.c_str())) {
          req.cseq = value;
        }
        req.headers[name] = value;
        state_ = token_expecting_new_line;
        return parse_more;
      } else if (is_ctl(input)) {
        return parse_fail;
      } else {
        value.push_back(input);
        return parse_more;
      }
    case token_expecting_last_line:
      if (input == '\n')
        return parse_done;
      else
        return parse_fail;
    default:
      return parse_fail;
    }
  }

  bool is_char(int c) { return c >= 0 && c <= 127; }

  bool is_ctl(int c) { return (c >= 0 && c <= 31) || (c == 127); }

  bool is_tspecial(int c) {
    switch (c) {
    case '(':
    case ')':
    case '<':
    case '>':
    case '@':
    case ',':
    case ';':
    case ':':
    case '\\':
    case '"':
    case '/':
    case '[':
    case ']':
    case '?':
    case '=':
    case '{':
    case '}':
    case ' ':
    case '\t':
      return true;
    default:
      return false;
    }
  }

  bool is_digit(int c) { return c >= '0' && c <= '9'; }

  state state_;

  std::string name;
  std::string value;
};
} // namespace details
} // namespace service
} // namespace aps
