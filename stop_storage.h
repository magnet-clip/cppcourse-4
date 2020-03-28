#pragma once

#include "geomath.h"
#include "stop.h"

#include <string>
#include <unordered_map>
#include <vector>

class StopStorage {
public:
  StopPtr AddQualifiedStop(const std::string &stop_name, GeoPoint location);
  StopPtr GetOrAddUnqualified(const std::string &stop_name);
  StopPtr TryGet(const std::string &stop_name) const;

private:
  std::unordered_map<std::string, StopPtr> _stops;
};