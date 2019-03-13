#include <Foundation/Foundation.h>

#include "../net_service_impl.h"

class net_service::net_service_impl : public net_service::implementation { 
public:
  net_service_impl(const std::string &type) {
    type_ = [NSString stringWithUTF8String:type.c_str()];
    txt_records_ = [NSMutableDictionary dictionaryWithCapacity:0];
  }

  ~net_service_impl() {}

  virtual void add_txt_record(const std::string &k,
                              const std::string &v) override {
    [txt_records_ setValue:[NSString stringWithUTF8String:v.c_str()]
                    forKey:[NSString stringWithUTF8String:k.c_str()]];
  }

  virtual bool publish(const std::string &name, const uint16_t port) override {
    NSString *n = [NSString stringWithUTF8String:name.c_str()];
    net_service_ =
        [[NSNetService alloc] initWithDomain:@"" type:type_ name:n port:port];
    NSData *txt_data = [NSNetService dataFromTXTRecordDictionary:txt_records_];
    [net_service_ setTXTRecordData:txt_data];
    [net_service_ publish];
    return true;
  }

  virtual void suppress() override {
    if (net_service_) {
      [net_service_ stop];
      [net_service_ release];
    }
  }

private:
  NSString *type_;
  NSNetService *net_service_;
  NSMutableDictionary *txt_records_;
};

net_service::implementation *
net_service::implementation::get(const std::string &type) {
  return new net_service_impl(type);
}
