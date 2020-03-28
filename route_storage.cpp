#include "route_storage.h"

using namespace std;

void RouteStorage::Add(const string &bus_name, bool is_circular,
                       const vector<StopId> &stop_names) {
  _routes.insert({bus_name, {is_circular, stop_names}});
}

vector<string> RouteStorage::GetBusesByStop(const StopId &stop_id) {
  vector<string> res;

  for (const auto &[bus_name, route] : _routes) {
    if (route.UniqueStops().count(stop_id)) {
      res.push_back(bus_name);
    }
  }

  return res;
}

optional<Route> RouteStorage::TryGet(const string &bus_name) {
  const auto it = _routes.find(bus_name);
  if (it != _routes.end()) {
    return it->second;
  }
  return nullopt;
}
