#pragma once

#include "geomath.h"

#include <set>
#include <string>
#include <vector>

class Stop {
public:
  Stop() {}
  Stop(const std::string &name, GeoPoint location)
      : _name(name), _location(location) {}

  const std::string &GetName() const { return _name; }
  const GeoPoint &GetLocation() const { return _location; }

  void AddBus(std::string bus_name) { _buses.insert(bus_name); }
  std::vector<std::string> GetUniqueBusNames() const {
    return {_buses.begin(), _buses.end()};
  }

private:
  std::string _name;
  GeoPoint _location;
  std::set<std::string> _buses;
};
