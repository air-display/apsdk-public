#include "ap_server.h"
#include "ap_config.h"
#include "mdns/net_service.h"
#include "service/ap_airplay_service.h"
#include <memory>


using namespace aps::service;
using namespace aps::network;

namespace aps {
class ap_server::implementation {
public:
  implementation(const ap_config &config)
      : config_(config), airplay_net_service_("_airplay._tcp"),
        raop_net_service_("_raop._tcp"), airplay_tcp_service_(0) {}

  ~implementation() {}

  void set_handler(ap_handler_ptr hanlder) { ap_handler_ = hanlder; }

  bool start() {
    if (airplay_tcp_service_)
      return true;

    airplay_tcp_service_ = std::make_shared<ap_airplay_service>(config_, 9123);
    airplay_tcp_service_->set_handler(ap_handler_);

    if (!airplay_tcp_service_)
      return false;

    if (!airplay_tcp_service_->start()) {
      airplay_tcp_service_.reset();
      return false;
    }

    if (!initialize_net_service()) {
      airplay_tcp_service_->stop();
      airplay_tcp_service_.reset();
      return false;
    }

    return true;
  }

  void stop() {
    airplay_net_service_.suppress();
    raop_net_service_.suppress();

    if (airplay_tcp_service_) {
      airplay_tcp_service_->stop();
      airplay_tcp_service_.reset();
    }
  }

protected:
  bool initialize_net_service() {
    airplay_net_service_.add_txt_record("deviceId", config_.macAddress());
    airplay_net_service_.add_txt_record("features",
                                        config_.features_hex_string());
    airplay_net_service_.add_txt_record("model", config_.model());
    airplay_net_service_.add_txt_record("srcvers", config_.serverVersion());
    airplay_net_service_.add_txt_record("vv", config_.vv());
    airplay_net_service_.add_txt_record("pi", config_.pi());
    airplay_net_service_.add_txt_record("pk", config_.pk());
    airplay_net_service_.add_txt_record("flags", config_.flags());

    raop_net_service_.add_txt_record("am", config_.model());
    raop_net_service_.add_txt_record("cn", config_.audioCodecs());
    raop_net_service_.add_txt_record("et", config_.encryptionTypes());
    raop_net_service_.add_txt_record("ft", config_.features_hex_string());
    raop_net_service_.add_txt_record("md", config_.metadataTypes());
    raop_net_service_.add_txt_record("pk", config_.pk());
    raop_net_service_.add_txt_record("tp", config_.transmissionProtocol());
    raop_net_service_.add_txt_record("vs", config_.serverVersion());
    raop_net_service_.add_txt_record("vv", config_.vv());
    raop_net_service_.add_txt_record("vn", "65537");
    raop_net_service_.add_txt_record("da", "true");
    raop_net_service_.add_txt_record("sf", "0x04");

    if (airplay_net_service_.publish(config_.name(),
                                     airplay_tcp_service_->port())) {
      std::string rapo_name = config_.deviceID();
      rapo_name += "@";
      rapo_name += config_.name();

      if (raop_net_service_.publish(rapo_name, airplay_tcp_service_->port()))
        return true;

      airplay_net_service_.suppress();
    }

    return false;
  }

private:
  ap_config config_;

  ap_handler_ptr ap_handler_;

  net_service airplay_net_service_;

  net_service raop_net_service_;

  ap_airplay_service_ptr airplay_tcp_service_;
};

ap_server::ap_server(const aps::ap_config &config)
    : impl_(new implementation(config)) {}

ap_server::~ap_server() { impl_.reset(); }

void ap_server::set_handler(ap_handler_ptr hanlder) {
  impl_->set_handler(hanlder);
}

bool aps::ap_server::start() { return impl_->start(); }

void ap_server::stop() { impl_->stop(); }
} // namespace aps
