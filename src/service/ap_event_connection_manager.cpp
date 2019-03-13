#include <service/ap_event_connection_manager.h>

namespace aps {
namespace service {
aps::service::ap_event_connection_manager &ap_event_connection_manager::get() {
  static ap_event_connection_manager s_instance;
  return s_instance;
}

void ap_event_connection_manager::insert(const std::string &id,
                                         xtxp_connection_base_weak_ptr p) {
  std::lock_guard<std::mutex> l(mtx_);
  even_connection_map_[id] = p;
}

void ap_event_connection_manager::remove(const std::string &id) {
  std::lock_guard<std::mutex> l(mtx_);
  auto it = even_connection_map_.find(id);
  if (it != even_connection_map_.end()) {
    even_connection_map_.erase(it);
  }
}

xtxp_connection_base_weak_ptr
ap_event_connection_manager::get(const std::string &id) {
  std::lock_guard<std::mutex> l(mtx_);
  auto it = even_connection_map_.find(id);
  if (it != even_connection_map_.end()) {
    return it->second;
  }
  return xtxp_connection_base_weak_ptr();
}

ap_event_connection_manager::ap_event_connection_manager() {}

ap_event_connection_manager::~ap_event_connection_manager() {}

} // namespace service
} // namespace aps
