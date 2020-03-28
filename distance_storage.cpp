#include "distance_storage.h"

using namespace std;

void DistanceStorage::AddDistance(const StopPair &route, Distance distance) {
  if (_distances.count(route)) {
    auto &existing = _distances.at(route);
    if (!distance.implicit && existing.implicit) {
      existing = distance;
    } else if (!distance.implicit && !existing.implicit) {
      throw domain_error("There's already an explicit distance");
    }
  } else {
    _distances.insert({route, distance});
  }
}

void DistanceStorage::AddDistances(
    StopId stop_id, const vector<pair<StopId, double>> &distances) {
  for (const auto &[other_id, distance] : distances) {
    AddDistance({stop_id, other_id}, {distance, false});
    AddDistance({other_id, stop_id}, {distance, true});
  }
}
