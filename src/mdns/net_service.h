#ifndef MDNS_H
#define MDNS_H
#pragma once

#include <cstdint>
#include <memory>
#include <string>

class net_service {
public:
  net_service(const std::string &type);

  ~net_service();

  void add_txt_record(const std::string &k, const std::string &v);

  void add_txt_record(const std::string &k, const int &v);

  bool publish(const std::string &name, const uint16_t port);

  void suppress();

private:
  class implementation;
  std::unique_ptr<implementation> impl_;

  class net_service_impl;
};

#endif // MDNS_H
