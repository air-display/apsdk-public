#pragma once
#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <asio.hpp>

#include <ap_config.h>
#include <crypto/ap_crypto.h>
#include <network/xtxp_connection_base.h>

using namespace aps::network;

namespace aps {
namespace service {
class ap_media_http_connection
    : public xtxp_connection_base,
      public std::enable_shared_from_this<ap_media_http_connection> {
public:
  ap_media_http_connection(asio::io_context &io_ctx);
  ~ap_media_http_connection();

  void get_handler(const request &req, response &res);

  virtual void add_common_header(const request &req, response &res) override;

protected:
  void initialize_request_handlers();

  virtual std::shared_ptr<xtxp_connection_base> shared_from_self() override;

private:
};

typedef std::shared_ptr<ap_media_http_connection> ap_media_http_connection_ptr;
typedef std::weak_ptr<ap_media_http_connection>
    ap_media_http_connection_weak_ptr;

class ap_media_http_service
    : public tcp_service_base,
      public std::enable_shared_from_this<ap_media_http_service> {
public:
  ap_media_http_service(ap_config_ptr &config, uint16_t port = 0);

  ~ap_media_http_service();

protected:
  virtual tcp_connection_ptr prepare_new_connection() override;

private:
  ap_config_ptr config_;
};

typedef std::shared_ptr<ap_media_http_service> ap_media_http_service_ptr;

} // namespace service
} // namespace aps
