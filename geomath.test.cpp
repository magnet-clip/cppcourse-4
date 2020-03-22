#include "geomath.test.h"
#include "geomath.h"
#include "test_runner.h"

#include <cmath>

void TestDistance() {
  GeoPoint tolstopaltsevo{Latitude(55.611087), Longitude(37.20829)};
  GeoPoint marushkino{Latitude(55.595884), Longitude(37.209755)};
  double d = Planet::Earth.Distance(tolstopaltsevo, marushkino);
  ASSERT(fabs(d - 1692.9994) < 0.00001);
}