/* 
 *  File: net_service_impl.cpp
 *  Project: apsdk
 *  Created: Oct 25, 2018
 *  Author: Sheen Tian
 *  
 *  This file is part of apsdk (https://github.com/air-display/apsdk-public) 
 *  Copyright (C) 2018-2024 Sheen Tian 
 *  
 *  apsdk is free software: you can redistribute it and/or modify it under the terms 
 *  of the GNU General Public License as published by the Free Software Foundation, 
 *  either version 3 of the License, or (at your option) any later version.
 *  
 *  apsdk is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *  See the GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License along with Foobar. 
 *  If not, see <https://www.gnu.org/licenses/>.
 */

#include <dlfcn.h>

#include <utils/logger.h>
#include <utils/utils.h>

#include "../dns_sd.h"
#include "../net_service_impl.h"

class net_service::net_service_impl : public net_service::implementation {
public:
  net_service_impl(const std::string &type) : dns_service_(0) {
    type_ = type;
    TXTRecordCreate(&txt_records_, 0, 0);
  }

  ~net_service_impl() { TXTRecordDeallocate(&txt_records_); }

  virtual void add_txt_record(const std::string &k, const std::string &v) override {
    auto error = TXTRecordSetValue(&txt_records_, k.c_str(), v.length(), v.c_str());

    if (error)
      LOGE() << "Failed to add TXT record:" << k << " = " << v << ": " << error;
  }

  virtual bool publish(const std::string &name, const uint16_t port) override {
    auto error = DNSServiceRegister(&dns_service_,
                                    0,
                                    0,
                                    name.c_str(),
                                    type_.c_str(),
                                    0,
                                    0,
                                    htons(port),
                                    TXTRecordGetLength(&txt_records_),
                                    TXTRecordGetBytesPtr(&txt_records_),
                                    0,
                                    0);

    if (!error)
      return true;

    LOGE() << "Failed to register service: " << name << ": " << error;
    return false;
  }

  virtual void suppress() override {
    if (dns_service_) {
      DNSServiceRefDeallocate(dns_service_);
      dns_service_ = nullptr;
    }
  }

private:
  std::string type_;
  DNSServiceRef dns_service_;
  TXTRecordRef txt_records_;
};

net_service::implementation *net_service::implementation::get(const std::string &type) {
  return new net_service_impl(type);
}
