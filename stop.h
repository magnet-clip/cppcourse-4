#pragma once

#include "geomath.h"

#include <string>
#include <vector>

class Stop {
public:
  Stop(const std::string &name, GeoPoint location)
      : _name(name), _location(location) {}
  const std::string &GetName() const { return _name; }
  const GeoPoint &GetLocation() const { return _location; }

private:
  std::string _name;
  GeoPoint _location;
};
