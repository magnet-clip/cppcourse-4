#pragma once

#include "geomath.h"
#include "id.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

class Stop {
public:
  Stop(const std::string &name) : _name(name) {}
  const std::string &GetName() const { return _name; }
  virtual const GeoPoint &GetLocation() const {
    throw std::domain_error("Stop [" + _name + "] has no location specified");
  }
  void AddBus(BusId bus_id) { _buses.insert(bus_id); }
  std::vector<BusId> GetUniqueBuses() const {
    return {_buses.begin(), _buses.end()};
  }

private:
  std::string _name;
  std::unordered_set<BusId> _buses;
};

class QualifiedStop : public Stop {
public:
  QualifiedStop(const Stop &preceder, GeoPoint location)
      : Stop(preceder), _location(location) {}

  QualifiedStop(const std::string &name, GeoPoint location)
      : Stop(name), _location(location) {}

  virtual const GeoPoint &GetLocation() const override { return _location; }

private:
  GeoPoint _location;
};

using StopPtr = std::shared_ptr<Stop>;

class StopPair {
public:
  StopPair(StopId first, StopId second) : _first(first), _second(second) {}

  const StopId &GetFirst() const { return _first; }
  const StopId &GetSecond() const { return _second; }

private:
  StopId _first;
  StopId _second;
};

bool operator==(const Stop &a, const Stop &b);
bool operator==(const StopPair &a, const StopPair &b);

struct StopPairHasher {
  size_t operator()(const StopPair &pair) const {
    size_t res = pair.GetFirst();
    res = 37 * res + pair.GetSecond();
    return res;
  }
};