#include "route_storage.h"

using namespace std;

void RouteStorage::Add(BusId bus_id, bool is_circular,
                       const vector<StopId> &stop_names) {
  _routes.insert({bus_id, {is_circular, stop_names}});
}

vector<BusId> RouteStorage::GetBusesByStop(StopId stop_id) {
  vector<BusId> res;

  for (const auto &[bus_id, route] : _routes) {
    if (route.UniqueStops().count(stop_id)) {
      res.push_back(bus_id);
    }
  }

  return res;
}

optional<Route> RouteStorage::TryGet(BusId bus_id) {
  const auto it = _routes.find(bus_id);
  if (it != _routes.end()) {
    return it->second;
  }
  return nullopt;
}
