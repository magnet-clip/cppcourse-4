#pragma once

#include "geomath.h"
#include "stop.h"
#include <vector>

class Route {
public:
  Route(const Planet &planet, bool circular, std::vector<Stop> stops);
  double Length() const { return _length; }
  int StopsCount() const { return _stops.size(); }
  int UniqueStopsCount() const { return _unique_stops_count; }

private:
  const Planet &_planet;
  bool _circular;
  double _length;
  int _unique_stops_count;
  std::vector<Stop> _stops;
};
