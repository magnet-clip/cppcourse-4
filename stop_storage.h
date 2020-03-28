#pragma once

#include "geomath.h"
#include "id.h"
#include "stop.h"

#include <string>
#include <unordered_map>
#include <vector>

class StopStorage {
public:
  StopId AddQualifiedStop(const std::string &stop_name, GeoPoint location);

  std::vector<StopId>
  GetOrAddUnqualifiedBulk(const std::vector<std::string> &stop_names);
  StopId GetOrAddUnqualified(const std::string &stop_name);

  StopPtr Get(StopId id) const { return _stops[id]; }
  std::optional<StopId> TryFindIdByName(const std::string &stop_name) const;
  StopPtr TryGetByName(const std::string &stop_name) const;

private:
  std::unordered_map<std::string, StopId> _stop_ids;
  std::vector<StopPtr> _stops;
};