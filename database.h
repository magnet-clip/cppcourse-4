#pragma once

#include "command.h"
#include "query.h"
#include "response.h"
#include "route.h"

#include <memory>
#include <vector>

class Database {
public:
  Database(std::vector<CommandPtr> commands);
  BusResponse ExecuteBusQuery(const BusQuery &query);

private:
  unordered_map<string, GeoPoint> _stops;
  unordered_map<int, RoutePtr> _routes;
};