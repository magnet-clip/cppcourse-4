#pragma once

#include "geomath.h"
#include <vector>

class Route {
public:
  Route(bool circular, std::vector<std::string> stops);
  double Length() const { return _length; }
  int StopsCount() const { return _stops.size(); }
  int UniqueStopsCount() const { return _unique_stops_count; }

private:
  bool _circular;
  double _length;
  int _unique_stops_count;
  std::vector<std::string> _stops;
};
