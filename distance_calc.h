#pragma once

#include "distance.h"
#include "distance_storage.h"
#include "geomath.h"
#include "route.h"
#include "stop.h"
#include "stop_storage.h"

#include <string>
#include <unordered_map>

template <class T> struct DistanceCalculator {
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