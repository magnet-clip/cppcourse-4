#pragma once

#include "geomath.h"
#include <set>
#include <vector>

class Route {
public:
  Route(bool circular, std::vector<std::string> stops);
  const std::vector<std::string> GetStops() const { return _stops; };
  const std::set<std::string> &UniqueStops() const { return _unique_stops; }

private:
  std::vector<std::string> _stops;
  std::set<std::string> _unique_stops;
};
