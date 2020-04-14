#pragma once

#include "id.h"

#include <vector>

struct BusAndRouteInfo {
  BusId bus_id;
  bool circular;
  double average_velocity;
  double average_wait_time;
  std::vector<StopId> stops;
  std::vector<std::tuple<StopId, StopId, double>> distances;
};

class MapStorage {
public:
private:
};

class MapStorageBuilder {
public:
  void AddRouteInfo(const BusAndRouteInfo &info) {}
  MapStorage Build() const {
    return {};
  }

private:
};
