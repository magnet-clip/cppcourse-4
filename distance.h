#pragma once

struct Distance {
  double distance;
  bool implicit;
  operator double() { return distance; }
};
