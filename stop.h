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

class StopPair {
public:
  explicit StopPair(Stop stop) : _first(stop), _second({}) {}
  StopPair(Stop first, Stop second)
      : _first(first.GetName() < second.GetName() ? first : second),
        _second(first.GetName() < second.GetName() ? second : first) {}

  const Stop &GetFirst() const { return _first; }
  const std::optional<Stop> &GetSecond() const { return _second; }

private:
  Stop _first;
  std::optional<Stop> _second;
};

bool operator==(const Stop &a, const Stop &b);
bool operator==(const StopPair &a, const StopPair &b);

struct StopPairHasher {
  std::hash<std::string> str_hasher;
  size_t operator()(const StopPair &pair) const {
    size_t res = str_hasher(pair.GetFirst().GetName());
    const auto &second = pair.GetSecond();
    if (second) {
      res = 37 * res + str_hasher(second->GetName());
    }
    return res;
  }
};