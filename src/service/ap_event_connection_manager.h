#pragma once
#include <map>
#include <memory>
#include <string>

#include <network/xtxp_connection_base.h>

using namespace aps::network;

namespace aps {
namespace service {
/// <summary>
/// Represents the manager of the reversed HTTP connections.
/// </summary>
class ap_event_connection_manager {
  typedef std::map<std::string, xtxp_connection_base_weak_ptr>
      event_connection_map;

public:
  static ap_event_connection_manager &get();

  void insert(const std::string &id, xtxp_connection_base_weak_ptr p);
  void remove(const std::string &id);
  xtxp_connection_base_weak_ptr get(const std::string &id);

protected:
  ap_event_connection_manager();
  ~ap_event_connection_manager();

private:
  event_connection_map even_connection_map_;
  std::mutex mtx_;
};

} // namespace service
} // namespace aps
