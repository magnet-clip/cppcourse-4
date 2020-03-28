#pragma once

#include "distance.h"
#include "id.h"
#include "stop.h"

#include <string>
#include <unordered_map>

class DistanceStorage {
public:
  void AddDistances(StopId stop_id,
                    const std::vector<std::pair<StopId, double>> &distances);
  double Get(StopPair pair) const { return _distances.at(pair).distance; }

private:
  void AddDistance(const StopPair &route, Distance distance);

  std::unordered_map<StopPair, Distance, StopPairHasher> _distances;
};
