#pragma once

#include "id.h"
#include "route.h"

#include <string>
#include <unordered_map>
#include <vector>

class RouteStorage {
public:
  void Add(BusId bus_id, bool is_circular, const std::vector<StopId> &stops);

  std::vector<BusId> GetBusesByStop(StopId stop_id);

  std::optional<Route> TryGet(BusId bus_id);
  Route Get(BusId bus_id) { return _routes.at(bus_id); }

private:
  std::unordered_map<BusId, Route> _routes;
};