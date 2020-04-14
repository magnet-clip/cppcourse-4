#pragma once

#include "distance.h"
#include "distance_storage.h"
#include "geomath.h"
#include "route.h"
#include "stop.h"
#include "stop_storage.h"

#include <string>
#include <unordered_map>

template <class T>
struct DistanceCalculator {
  double Calculate(const Route &route) {
    double res = 0.0;

    route.IterateByPair([this, &res](StopId prev, StopId curr) {
      const auto &prev_point = GetCurrent(prev);
      const auto &curr_point = GetCurrent(curr);
      res += GetDistance(prev_point, curr_point);
    });

    return res;
  }

  virtual double GetDistance(T a, T b) const = 0;

protected:
  virtual T GetCurrent(StopId stop_id) const = 0;
};

class HelicopterDistanceCalculator : public DistanceCalculator<GeoPoint> {
public:
  HelicopterDistanceCalculator(const Planet &planet, const StopStorage &stops)
      : _planet(planet), _stops(stops) {}

  virtual double GetDistance(GeoPoint a, GeoPoint b) const override {
    return _planet.Distance(a, b);
  }

protected:
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

  virtual double GetDistance(StopId a, StopId b) const override {
    return _distances.Get({a, b});
  }

protected:
  virtual StopId GetCurrent(StopId stop_id) const override { return stop_id; }

private:
  const DistanceStorage &_distances;
};