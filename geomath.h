#pragma once

#include <sstream>

#define NAMED_VALUE(name, type)                                                \
  class name {                                                                 \
  public:                                                                      \
    explicit name(type value) : _value(value) {}                               \
    name &operator=(name other) {                                              \
      _value = other._value;                                                   \
      return *this;                                                            \
    }                                                                          \
    name &operator=(type value) {                                              \
      _value = value;                                                          \
      return *this;                                                            \
    }                                                                          \
    operator double() const { return _value; }                                 \
    bool operator==(name other) { return _value == other._value; }             \
                                                                               \
  private:                                                                     \
    type _value;                                                               \
  }

NAMED_VALUE(Latitude, double);
NAMED_VALUE(Longitude, double);

class GeoPoint {
public:
  GeoPoint() : _latitude(0), _longitude(0) {}
  GeoPoint(Latitude latitude, Longitude longitude)
      : _latitude(latitude), _longitude(longitude) {}

  Latitude GetLatitude() const { return _latitude; }
  Longitude GetLongitude() const { return _longitude; }
  void SetLatitude(double latitude) { _latitude = latitude; }
  void SetLongitude(double longitude) { _longitude = longitude; }

private:
  Latitude _latitude;
  Longitude _longitude;
};

bool operator==(GeoPoint a, GeoPoint b);
std::istream &operator>>(std::istream &is, GeoPoint &point);
std::ostream &operator<<(std::ostream &os, GeoPoint point);

class Planet {
public:
  static Planet Earth;
  double Distance(GeoPoint a, GeoPoint b);

private:
  explicit Planet(double radius) : _radius(radius) {}
  const double PI = 3.1415926535;
  const double _radius;
};
