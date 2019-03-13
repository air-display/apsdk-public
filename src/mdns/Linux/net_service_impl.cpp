#include <dlfcn.h>

#include <utils/logger.h>
#include <utils/utils.h>

#include "../net_service_impl.h"

class net_service::net_service_impl : public net_service::implementation {
public:
  net_service_impl(const std::string &type) { type_ = type; }

  ~net_service_impl() {}

  virtual void add_txt_record(const std::string &k,
                              const std::string &v) override {}

  virtual bool publish(const std::string &name, const uint16_t port) override {

    LOGE() << "Failed to register service: " << name << ": " << 0;
    return false;
  }

  virtual void suppress() override {}

private:
  std::string type_;
};

net_service::implementation *
net_service::implementation::get(const std::string &type) {
  return new net_service_impl(type);
}
