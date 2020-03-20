#pragma once

#include <sstream>

#define NAMED_VALUE(name, type)                                                \
  class name {                                                                 \
  public:                                                                      \
    explicit name(type value) : _value(value) {}                               \
    operator double() const { return _value; }                                 \
    bool operator==(name other) { return _value == other._value; }             \
                                                                               \
  private:                                                                     \
    type _value;                                                               \
  }

NAMED_VALUE(Latitude, double);
NAMED_VALUE(Longitude, double);

struct GeoPoint {
  GeoPoint() : latitude(0), longitude(0) {}
  Latitude latitude;
  Longitude longitude;
};

bool operator==(GeoPoint a, GeoPoint b);
std::istream &operator>>(std::istream &is, GeoPoint &point);

class Planet {
public:
  static Planet Earth;
  double Distance(GeoPoint a, GeoPoint b);

private:
  explicit Planet(double radius) : _radius(radius) {}
  const double PI = 3.1415926535;
  const double _radius;
};
