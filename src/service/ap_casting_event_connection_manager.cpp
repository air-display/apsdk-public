#include <service/ap_casting_event_connection_manager.h>

#include <utility>

namespace aps {
namespace service {
aps::service::ap_casting_event_connection_manager &ap_casting_event_connection_manager::get() {
  static ap_casting_event_connection_manager s_instance;
  return s_instance;
}

void ap_casting_event_connection_manager::insert(const std::string &id, xtxp_connection_base_weak_ptr p) {
  std::lock_guard<std::mutex> l(mtx_);
  even_connection_map_[id] = std::move(p);
}

void ap_casting_event_connection_manager::remove(const std::string &id) {
  std::lock_guard<std::mutex> l(mtx_);
  auto it = even_connection_map_.find(id);
  if (it != even_connection_map_.end()) {
    even_connection_map_.erase(it);
  }
}

xtxp_connection_base_weak_ptr ap_casting_event_connection_manager::get(const std::string &id) {
  std::lock_guard<std::mutex> l(mtx_);
  auto it = even_connection_map_.find(id);
  if (it != even_connection_map_.end()) {
    return it->second;
  }
  return xtxp_connection_base_weak_ptr();
}

ap_casting_event_connection_manager::ap_casting_event_connection_manager() = default;

ap_casting_event_connection_manager::~ap_casting_event_connection_manager() = default;

} // namespace service
} // namespace aps
