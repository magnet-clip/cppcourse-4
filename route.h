#pragma once

#include "geomath.h"
#include "id.h"

#include <set>
#include <vector>

class Route {
public:
  Route(bool circular, std::vector<StopId> stops);
  const std::vector<StopId> GetStopIds() const { return _stops; };
  const std::set<StopId> &UniqueStops() const { return _unique_stops; }

private:
  std::vector<StopId> _stops;
  std::set<StopId> _unique_stops;
};
