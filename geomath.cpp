#include "geomath.h"

#include <cmath>

using namespace std;

Planet Planet::Earth(6371);

double Planet::Distance(GeoPoint a, GeoPoint b) {
  double lat1 = a.GetLatitude() * PI / 180.0;
  double lon1 = a.GetLongitude() * PI / 180.0;
  double lat2 = b.GetLatitude() * PI / 180.0;
  double lon2 = b.GetLongitude() * PI / 180.0;

  return acos(sin(lat1) * sin(lat2) +
              cos(lat1) * cos(lat2) * cos(abs(lon1 - lon2))) *
         _radius * 1000;
}

istream &operator>>(istream &is, GeoPoint &point) {
  double lat = 0.0, lon = 0.0;
  is >> lat;
  is.ignore(1);
  is >> lon;
  point.SetLongitude(lon);
  point.SetLatitude(lat);
  return is;
}
ostream &operator<<(ostream &os, GeoPoint point) {
  return os << "(" << point.GetLatitude() << ", " << point.GetLongitude()
            << ")";
}

bool operator==(GeoPoint a, GeoPoint b) {
  return (a.GetLatitude() == b.GetLatitude()) &&
         (a.GetLongitude() == b.GetLongitude());
}
