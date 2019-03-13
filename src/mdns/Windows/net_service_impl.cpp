#include <utils/logger.h>

#include "../dns_sd.h"
#include "../net_service_impl.h"

class net_service::net_service_impl : public net_service::implementation {
  typedef DNSServiceErrorType(DNSSD_API *DNSServiceRegister_t)(
      DNSServiceRef *sdRef, DNSServiceFlags flags, uint32_t interfaceIndex,
      const char *name, const char *regtype, const char *domain,
      const char *host, uint16_t port, uint16_t txtLen, const void *txtRecord,
      DNSServiceRegisterReply callBack, void *context);

  typedef void(DNSSD_API *DNSServiceRefDeallocate_t)(DNSServiceRef sdRef);

  typedef void(DNSSD_API *TXTRecordCreate_t)(TXTRecordRef *txtRecord,
                                             uint16_t bufferLen, void *buffer);

  typedef void(DNSSD_API *TXTRecordDeallocate_t)(TXTRecordRef *txtRecord);

  typedef DNSServiceErrorType(DNSSD_API *TXTRecordSetValue_t)(
      TXTRecordRef *txtRecord, const char *key, uint8_t valueSize,
      const void *value);

  typedef uint16_t(DNSSD_API *TXTRecordGetLength_t)(
      const TXTRecordRef *txtRecord);

  typedef const void *(DNSSD_API *TXTRecordGetBytesPtr_t)(
      const TXTRecordRef *txtRecord);

  typedef struct dnssd_lib_s {
    HMODULE Module;
    DNSServiceRegister_t DNSServiceRegister;
    DNSServiceRefDeallocate_t DNSServiceRefDeallocate;
    TXTRecordCreate_t TXTRecordCreate;
    TXTRecordSetValue_t TXTRecordSetValue;
    TXTRecordGetLength_t TXTRecordGetLength;
    TXTRecordGetBytesPtr_t TXTRecordGetBytesPtr;
    TXTRecordDeallocate_t TXTRecordDeallocate;
  } dnssd_lib_t;

public:
  net_service_impl(const std::string &type) : dns_service_(0) {
    type_ = type;
    dnssd_lib_.Module = ::LoadLibraryA("dnssd.dll");
    if (dnssd_lib_.Module) {
      dnssd_lib_.DNSServiceRegister = (DNSServiceRegister_t)GetProcAddress(
          dnssd_lib_.Module, "DNSServiceRegister");
      dnssd_lib_.DNSServiceRefDeallocate =
          (DNSServiceRefDeallocate_t)GetProcAddress(dnssd_lib_.Module,
                                                    "DNSServiceRefDeallocate");
      dnssd_lib_.TXTRecordCreate = (TXTRecordCreate_t)GetProcAddress(
          dnssd_lib_.Module, "TXTRecordCreate");
      dnssd_lib_.TXTRecordSetValue = (TXTRecordSetValue_t)GetProcAddress(
          dnssd_lib_.Module, "TXTRecordSetValue");
      dnssd_lib_.TXTRecordGetLength = (TXTRecordGetLength_t)GetProcAddress(
          dnssd_lib_.Module, "TXTRecordGetLength");
      dnssd_lib_.TXTRecordGetBytesPtr = (TXTRecordGetBytesPtr_t)GetProcAddress(
          dnssd_lib_.Module, "TXTRecordGetBytesPtr");
      dnssd_lib_.TXTRecordDeallocate = (TXTRecordDeallocate_t)GetProcAddress(
          dnssd_lib_.Module, "TXTRecordDeallocate");

      dnssd_lib_.TXTRecordCreate(&txt_records_, 0, 0);
    } else
      LOGE() << "Failed to load dnssd.dll. Please confirm the Bonjour Service "
                "has been installed successfully.";
  }

  ~net_service_impl() {
    if (dnssd_lib_.Module) {
      dnssd_lib_.TXTRecordDeallocate(&txt_records_);

      ::FreeLibrary(dnssd_lib_.Module);
      dnssd_lib_.Module = 0;
    }
  }

  virtual void add_txt_record(const std::string &k,
                              const std::string &v) override {
    if (dnssd_lib_.Module && dnssd_lib_.TXTRecordSetValue) {
      auto error = dnssd_lib_.TXTRecordSetValue(&txt_records_, k.c_str(),
                                                (uint8_t)v.length(), v.c_str());

      if (error)
        LOGE() << "Failed to add TXT record:" << k << " = " << v << ": "
               << error;
    }
  }

  virtual bool publish(const std::string &name, const uint16_t port) override {
    if (dnssd_lib_.Module && dnssd_lib_.DNSServiceRegister &&
        dnssd_lib_.TXTRecordGetLength && dnssd_lib_.TXTRecordGetBytesPtr) {
      auto error = dnssd_lib_.DNSServiceRegister(
          &dns_service_, 0, 0, name.c_str(), type_.c_str(), 0, 0, htons(port),
          dnssd_lib_.TXTRecordGetLength(&txt_records_),
          dnssd_lib_.TXTRecordGetBytesPtr(&txt_records_), 0, 0);

      if (error)
        LOGE() << "Failed to register service: " << name << ": " << error;
      else
        return true;
    }

    return false;
  }

  virtual void suppress() override {
    if (dns_service_ && dnssd_lib_.Module &&
        dnssd_lib_.DNSServiceRefDeallocate) {
      dnssd_lib_.DNSServiceRefDeallocate(dns_service_);
      dns_service_ = 0;
    }
  }

private:
  std::string type_;
  DNSServiceRef dns_service_;
  TXTRecordRef txt_records_;
  dnssd_lib_t dnssd_lib_;
};

net_service::implementation *
net_service::implementation::get(const std::string &type) {
  return new net_service_impl(type);
}
