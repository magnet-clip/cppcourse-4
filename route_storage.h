#pragma once

#include "id.h"
#include "route.h"

#include <string>
#include <unordered_map>
#include <vector>

class RouteStorage {
public:
  void Add(const std::string &bus_name, bool is_circular,
           const std::vector<StopId> &stops);

  std::vector<std::string> GetBusesByStop(const StopId &stop_id);

  std::optional<Route> TryGet(const std::string &bus_name);

private:
  std::unordered_map<std::string, Route> _routes;
};