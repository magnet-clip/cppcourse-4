#pragma once

#include "geomath.h"
#include "id.h"

#include <set>
#include <vector>

class Route {
public:
  Route(bool circular, std::vector<StopId> stops);
  bool IsCircular() const { return _circular; };

  template <class TCallback>
  void IterateByPair(const TCallback &callback) const {
    std::optional<StopId> prev = std::nullopt;
    std::optional<StopId> current = std::nullopt;
    for (const auto next_stop_id : _stops) {
      if (prev && current) {
        callback(*prev, *current);
      }
      prev = current;
      current = next_stop_id;
    }
    callback(*prev, *current);
  }

  const std::vector<StopId> &Stops() const { return _stops; };
  size_t StopsCount() const { return _stops.size(); };
  const std::set<StopId> &UniqueStops() const { return _unique_stops; }
  size_t UniqueStopsCount() const { return _unique_stops.size(); }

private:
  std::vector<StopId> _stops;
  std::set<StopId> _unique_stops;
  bool _circular;
};
