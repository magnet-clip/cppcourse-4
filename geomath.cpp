#include "geomath.h"

#include <cmath>

using namespace std;

Planet Planet::Earth(6371);

double Planet::Distance(GeoPoint a, GeoPoint b) {
  double lat1 = a.latitude * PI / 180.0;
  double lon1 = a.longitude * PI / 180.0;
  double lat2 = b.latitude * PI / 180.0;
  double lon2 = b.longitude * PI / 180.0;

  return acos(sin(lat1) * sin(lat2) +
              cos(lat1) * cos(lat2) * cos(abs(lon1 - lon2))) *
         _radius * 1000;
}

istream &operator>>(istream &is, GeoPoint &point) {
  double lat = 0.0, lon = 0.0;
  is >> lat;
  is.ignore(1);
  is >> lon;
  point.longitude = Longitude(lon);
  point.latitude = Latitude(lat);
  return is;
}

bool operator==(GeoPoint a, GeoPoint b) {
  return (a.latitude == b.latitude) && (a.longitude == b.longitude);
}
