#include "net_service.h"
#include "net_service_impl.h"

net_service::net_service(const std::string &type)
    : impl_(implementation::get(type)) {}

net_service::~net_service() {}

void net_service::add_txt_record(const std::string &k, const std::string &v) {
  if (impl_)
    impl_->add_txt_record(k, v);
}

void net_service::add_txt_record(const std::string &k, const int &v) {
  if (impl_)
    impl_->add_txt_record(k, std::to_string(v));
}

bool net_service::publish(const std::string &name, const uint16_t port) {
  if (impl_)
    return impl_->publish(name, port);

  return false;
}

void net_service::suppress() {
  if (impl_)
    return impl_->suppress();
}
