#pragma once

#include "distance.h"
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
    for (const auto next_stop_name : route.GetStopNames()) {
      if (prev && current) {
        res += GetDistance(*prev, *current);
      }
      prev = current;
      current = GetCurrent(next_stop_name);
    }
    res += GetDistance(*prev, *current);
    return res;
  }

protected:
  virtual double GetDistance(const T &a, const T &b) const = 0;
  virtual const T &GetCurrent(const std::string &stop_name) const = 0;
};

class HelicopterDistanceCalculator : public DistanceCalculator<GeoPoint> {
public:
  HelicopterDistanceCalculator(const Planet &planet, const StopStorage &stops)
      : _planet(planet), _stops(stops) {}

protected:
  virtual double GetDistance(const GeoPoint &a,
                             const GeoPoint &b) const override {
    return _planet.Distance(a, b);
  }

  virtual const GeoPoint &
  GetCurrent(const std::string &stop_name) const override {
    return _stops.TryGet(stop_name)->GetLocation();
  }

private:
  const Planet &_planet;
  const StopStorage &_stops;
};

class GivenDistanceCalculator : public DistanceCalculator<std::string> {
public:
  GivenDistanceCalculator(
      const std::unordered_map<StopPair, Distance, StopPairHasher> *distances)
      : _distances(distances) {}

protected:
  virtual const std::string &
  GetCurrent(const std::string &stop_name) const override {
    return stop_name;
  }

  virtual double GetDistance(const std::string &a,
                             const std::string &b) const override {
    return _distances->at({a, b}).distance;
  }

private:
  const std::unordered_map<StopPair, Distance, StopPairHasher> *_distances;
};