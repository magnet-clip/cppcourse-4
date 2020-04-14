#pragma once

#include "distance.h"
#include "distance_storage.h"
#include "geomath.h"
#include "route.h"
#include "stop.h"
#include "stop_storage.h"

#include <string>
#include <unordered_map>

template <typename T>
class DistanceCalculator;

template <typename T>
class DistanceIterator;

struct DistanceInfo {
  StopId first;
  StopId second;
  double distance;
};

template <typename T>
class DistanceLooper {
public:
  DistanceLooper(const Route &route, const DistanceCalculator<T> &calc) : _route(route), _calc(calc) {}
  DistanceIterator<T> begin() const {
    return {_route, 0, _calc};
  }
  DistanceIterator<T> end() const {
    return {_route, _route.GetStopsCount(), _calc};
  }

private:
  const Route &_route;
  const DistanceCalculator<T> &_calc;
};

template <class T>
class DistanceIterator {
public:
  DistanceIterator(const Route &route, size_t pos, const DistanceCalculator<T> &calc) : _stops(route.GetStopIds()), _pos(pos), _calc(calc) {}

  DistanceInfo operator*() {
    auto a = this->_calc.GetCurrent(_stops[_pos]);
    auto b = _calc.GetCurrent(_stops[_pos + 1]);
    auto d = _calc.GetDistance(a, b);
    return {_stops[_pos], _stops[_pos + 1], 0};
  }

  DistanceIterator operator++() {
    _pos++;
    return *this;
  }

  operator int() const { return _pos; }

private:
  const std::vector<StopId> &_stops;
  const DistanceCalculator<T> &_calc;
  size_t _pos;
};

template <class T>
struct DistanceCalculator {

  DistanceLooper<T> GetDistance(const Route &route) const {
    return {route, *this};
  };

  double Calculate(const Route &route) {
    double res = 0.0;

    std::optional<T> prev = std::nullopt;
    std::optional<T> current = std::nullopt;
    for (const auto next_stop_id : route.GetStopIds()) {
      if (prev && current) {
        res += GetDistance(*prev, *current);
      }
      prev = current;
      current = GetCurrent(next_stop_id);
    }
    res += GetDistance(*prev, *current);
    return res;
  }

protected:
  virtual double GetDistance(T a, T b) const = 0;
  virtual T GetCurrent(StopId stop_id) const = 0;
};

class HelicopterDistanceCalculator : public DistanceCalculator<GeoPoint> {
public:
  HelicopterDistanceCalculator(const Planet &planet, const StopStorage &stops)
      : _planet(planet), _stops(stops) {}

protected:
  virtual double GetDistance(GeoPoint a, GeoPoint b) const override {
    return _planet.Distance(a, b);
  }

  virtual GeoPoint GetCurrent(StopId stop_id) const override {
    return _stops.Get(stop_id)->GetLocation();
  }

private:
  const Planet &_planet;
  const StopStorage &_stops;
};

class GivenDistanceCalculator : public DistanceCalculator<StopId> {
public:
  GivenDistanceCalculator(const DistanceStorage &distances)
      : _distances(distances) {}

protected:
  virtual StopId GetCurrent(StopId stop_id) const override { return stop_id; }

  virtual double GetDistance(StopId a, StopId b) const override {
    return _distances.Get({a, b});
  }

private:
  const DistanceStorage &_distances;
};