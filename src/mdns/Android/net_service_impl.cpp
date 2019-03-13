#include <dlfcn.h>
#include <sys/endian.h>

#include <utils/logger.h>

#include "../dns_sd.h"
#include "../net_service_impl.h"

class net_service::net_service_impl : public net_service::implementation {
public:
  net_service_impl(const std::string &type) : dns_service_(0) {
    type_ = type;
    TXTRecordCreate(&txt_records_, 0, 0);
  }

  ~net_service_impl() { TXTRecordDeallocate(&txt_records_); }

  virtual void add_txt_record(const std::string &k,
                              const std::string &v) override {
    auto error =
        TXTRecordSetValue(&txt_records_, k.c_str(), v.length(), v.c_str());

    if (error)
      LOGE() << "Failed to add TXT record:" << k << " = " << v << ": " << error;
  }

  virtual bool publish(const std::string &name, const uint16_t port) override {
    auto error =
        DNSServiceRegister(&dns_service_, 0, 0, name.c_str(), type_.c_str(), 0,
                           0, htons(port), TXTRecordGetLength(&txt_records_),
                           TXTRecordGetBytesPtr(&txt_records_), 0, 0);

    if (!error)
      return true;

    LOGE() << "Failed to register service: " << name << ": " << error;
    return false;
  }

  virtual void suppress() override {
    if (dns_service_) {
      DNSServiceRefDeallocate(dns_service_);
    }
  }

private:
  std::string type_;
  DNSServiceRef dns_service_;
  TXTRecordRef txt_records_;
};

net_service::implementation *
net_service::implementation::get(const std::string &type) {
  return new net_service_impl(type);
}
