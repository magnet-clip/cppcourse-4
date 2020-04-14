#pragma once

#include "geomath.h"
#include "id.h"

#include <set>
#include <vector>

class Route {
public:
  Route(bool circular, std::vector<StopId> stops);
  bool IsCircular() const { return _circular; };

  // todo circular / non-circular?
  const std::vector<StopId> &GetStopIds() const { return _stops; };
  size_t GetStopsCount() const { return _stops.size(); };
  const std::set<StopId> &UniqueStops() const { return _unique_stops; }
  size_t GetUniqueStopsCount() const { return _unique_stops.size(); }

private:
  std::vector<StopId> _stops;
  std::set<StopId> _unique_stops;
  bool _circular;
};
