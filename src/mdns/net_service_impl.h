#include "net_service.h"

class net_service::implementation {
public:
  static implementation *get(const std::string &type);

  virtual ~implementation(){};

  virtual void add_txt_record(const std::string &k, const std::string &v) = 0;

  virtual bool publish(const std::string &name, const uint16_t port) = 0;

  virtual void suppress() = 0;
};
