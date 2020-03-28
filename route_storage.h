#pragma once

#include "route.h"

#include <string>
#include <unordered_map>
#include <vector>

// Route contains stops, and currently I store strings instead of
// references to stops. That's duplication and source of inconsistency
// TODO refactor route to store StopPtr's.
// TODO should I WeakPtrs maybe???
// TODO stop can be given a unique id instead of string name
class RouteStorage {
public:
  // TODO switch to StopPtrs / stop_ids instead of names
  void Add(const std::string &bus_name, bool is_circular,
           const std::vector<std::string> &stop_names);

  // TODO switch to StopPtrs / stop_ids instead of names
  std::vector<std::string> GetBusesByStop(const std::string &stop_name);

  std::optional<Route> TryGet(const std::string &bus_name);

private:
  std::unordered_map<std::string, Route> _routes;
};