#pragma once

#include "geomath.h"

#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

class Stop {
public:
  Stop(const std::string &name) : _name(name) {}
  const std::string &GetName() const { return _name; }
  virtual const GeoPoint &GetLocation() const {
    throw std::domain_error("Stop [" + _name + "] has no location specified");
  }
  void AddBus(std::string bus_name) { _buses.insert(bus_name); }
  std::vector<std::string> GetUniqueBusNames() const {
    return {_buses.begin(), _buses.end()};
  }

private:
  std::string _name;
  std::set<std::string> _buses;
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
  explicit StopPair(const std::string &stop_name)
      : _first(stop_name), _second({}) {}

  StopPair(const std::string &first, const std::string &second)
      : _first(first < second ? first : second),
        _second(first < second ? second : first) {}

  const std::string &GetFirst() const { return _first; }
  const std::optional<std::string> &GetSecond() const { return _second; }

private:
  std::string _first;
  std::optional<std::string> _second;
};

bool operator==(const Stop &a, const Stop &b);
bool operator==(const StopPair &a, const StopPair &b);

struct StopPairHasher {
  std::hash<std::string> str_hasher;
  size_t operator()(const StopPair &pair) const {
    size_t res = str_hasher(pair.GetFirst());
    const auto &second = pair.GetSecond();
    if (second) {
      res = 37 * res + str_hasher(*second);
    }
    return res;
  }
};