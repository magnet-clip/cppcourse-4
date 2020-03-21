#pragma once

#include "geomath.h"

struct Route {
  double Length(const Planet &planet) const;
  virtual int StopsCount() const = 0;
  virtual int UniqueStopsCount() const = 0;
};

class DirectRoute : public Route {
  virtual int StopsCount() const override;
  virtual int UniqueStopsCount() const override;
};

class CircularRoute : public Route {
  virtual int StopsCount() const override;
  virtual int UniqueStopsCount() const override;
};

using RoutePtr = shared_ptr<Route>;