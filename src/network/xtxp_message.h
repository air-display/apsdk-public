#pragma once
#include <cstdint>
#include <cstring>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <utils/logger.h>

namespace aps {
namespace network {
// Line break and delimiter
static const char *RN_LINE_BREAK = "\r\n";
static const char *RNRN_LINE_BREAK = "\r\n\r\n";
static const char *CHAR_BLANK = " ";
static const char *CHAR_COLON = ":";

// Header keys
static const char *HEADER_CONTENT_TYPE = "Content-Type";
static const char *HEADER_CONTENT_LENGTH = "Content-Length";
static const char *HEADER_SERVER = "Server";
static const char *HEADER_DATE = "Date";
static const char *HEADER_ALLOW_ORIGIN = "Access-Control-Allow-Origin";
static const char *HEADER_ALLOW_HEADER = "Access-Control-Allow-Headers";

// Content type
static const char *APPLICATION_MPEGURL = "application/x-mpegURL; charset=utf-8";
static const char *APPLICATION_OCTET_STREAM = "application/octet-stream";
static const char *APPLICATION_BINARY_PLIST =
    "application/x-apple-binary-plist";
static const char *APPLICATION_DMAP_TAGGED = "application/x-dmap-tagged";
static const char *TEXT_APPLE_PLIST_XML = "text/x-apple-plist+xml";
static const char *TEXT_PARAMETERS = "text/parameters";
static const char *IMAGE_JPEG = "image/jpeg";
static const char *IMAGE_PNG = "image/png";

typedef std::map<std::string, std::string> header_map;

class xtxp_message {
public:
  std::string scheme_version;
  int content_length;
  std::string content_type;
  header_map headers;
  std::vector<uint8_t> content;

  xtxp_message();
  ;
  xtxp_message(const std::string &scm_ver);
  ;
  ~xtxp_message();
  ;

  xtxp_message &with_content_type(const std::string &type);

  xtxp_message &with_content(const std::vector<uint8_t> data);

  xtxp_message &with_content(const uint8_t *data, int length);

  xtxp_message &with_content(const std::string &data);

  xtxp_message &with_header(const std::string &name, const std::string &value);

  std::string serialize() const;
};

/// <summary>
///
/// </summary>
class request : public xtxp_message {
public:
  std::string method;
  std::string uri;

  request();

  request(const std::string &scheme_ver, const std::string &methot,
          const std::string &uri);

  std::string serialize() const;

  void dump(const std::string tag) const;
};

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
class response : public xtxp_message {
public:
  status_type_t status_code;
  std::string status_text;

  response();

  response(const std::string &scheme_ver);

  response &with_status(status_type_t code);

  std::string serialize() const;
};

#if defined(NDEBUG)
#define DUMP_REQUEST_WITH_CONNECTION(x)
#else
#define DUMP_REQUEST_WITH_CONNECTION(x)                                        \
  x.dump("[" + std::to_string((long long)this) + "]")
#endif // _DEBUG

/// <summary>
/// Represents the message parser of HTTP/RTSP request message.
/// </summary>
class http_message_parser {
  /// The current state of the parser.
  enum state {
    token_start,

    // for request
    token_method,
    token_uri,

    token_scheme_version,

    // for response
    token_status_code,
    token_status_text,

    token_expecting_new_line,
    token_header_line_start,
    token_header_lws,
    token_header_name,
    token_space_before_header_value,
    token_header_value,
    token_expecting_last_line,
  };

public:
  enum parse_result {
    parse_more,
    parse_done,
    parse_fail,
  };

  /// Construct ready to parse the request method.
  http_message_parser() : state_(token_start) {}

  /// Reset to initial parser state.
  void reset() { state_ = token_start; }

  bool parse(request &req, std::string data) {
    req.method.clear();
    req.uri.clear();
    req.scheme_version.clear();
    req.headers.clear();
    req.content.clear();
    req.content_length = 0;

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

  bool parse(response &res, std::string data) {
    res.scheme_version.clear();
    res.status_code = ok;
    res.status_text.clear();
    res.content_length = 0;
    res.content_type.clear();

    int i = 0;
    while (i != data.length()) {
      parse_result result = consume(res, data[i++]);
      if (parse_more != result)
        reset();

      if (parse_fail == result)
        return false;
    }
    return true;
  }

private:
  // Handle the next character of input.
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
        name_.clear();
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
      } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return parse_fail;
      } else {
        name_.push_back(input);
        state_ = token_header_name;
        return parse_more;
      }
    case token_header_name:
      if (input == ':') {
        value_.clear();
        state_ = token_space_before_header_value;
        return parse_more;
      } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return parse_fail;
      } else {
        name_.push_back(input);
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
        if (0 == std::strcmp(HEADER_CONTENT_LENGTH, name_.c_str())) {
          char *end = 0;
          req.content_length = std::strtol(value_.c_str(), &end, 10);
        }
        if (0 == std::strcmp(HEADER_CONTENT_TYPE, name_.c_str())) {
          req.content_type = value_;
        }
        req.headers[name_] = value_;
        state_ = token_expecting_new_line;
        return parse_more;
      } else if (is_ctl(input)) {
        return parse_fail;
      } else {
        value_.push_back(input);
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

  // Handle the next character of input.
  parse_result consume(response &res, char input) {
    switch (state_) {
    case token_start:
      if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return parse_fail;
      } else {
        res.scheme_version.push_back(input);
        state_ = token_scheme_version;
        return parse_more;
      }
    case token_scheme_version:
      if (input == ' ') {
        state_ = token_status_code;
        code_.clear();
        return parse_more;
      } else if (!is_char(input)) {
        return parse_fail;
      } else {
        res.scheme_version.push_back(input);
        return parse_more;
      }
    case token_status_code:
      if (input == ' ') {
        state_ = token_status_text;
        res.status_code = (status_type_t)std::strtol(code_.c_str(), 0, 10);
        return parse_more;
      } else if (!is_digit(input) || is_ctl(input)) {
        return parse_fail;
      } else {
        code_.push_back(input);
        return parse_more;
      }
    case token_status_text:
      if (input == '\r') {
        state_ = token_expecting_new_line;
        return parse_more;
      } else if (!is_char(input)) {
        return parse_fail;
      } else {
        res.status_text.push_back(input);
        return parse_more;
      }
    case token_expecting_new_line:
      if (input == '\n') {
        name_.clear();
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
      } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return parse_fail;
      } else {
        name_.push_back(input);
        state_ = token_header_name;
        return parse_more;
      }
    case token_header_name:
      if (input == ':') {
        state_ = token_space_before_header_value;
        return parse_more;
      } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return parse_fail;
      } else {
        name_.push_back(input);
        return parse_more;
      }
    case token_space_before_header_value:
      if (input == ' ') {
        state_ = token_header_value;
        value_.clear();
        return parse_more;
      } else {
        return parse_fail;
      }
    case token_header_value:
      if (input == '\r') {
        if (0 == std::strcmp(HEADER_CONTENT_LENGTH, name_.c_str())) {
          char *end = 0;
          res.content_length = std::strtol(value_.c_str(), &end, 10);
        }
        if (0 == std::strcmp(HEADER_CONTENT_TYPE, name_.c_str())) {
          res.content_type = value_;
        }
        res.headers[name_] = value_;
        state_ = token_expecting_new_line;
        return parse_more;
      } else if (is_ctl(input)) {
        return parse_fail;
      } else {
        value_.push_back(input);
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
  std::string code_;
  std::string name_;
  std::string value_;
};
} // namespace network
} // namespace aps
