#pragma once

#include "command.h"
#include "query.h"
#include "response.h"
#include "route.h"

#include <memory>
#include <unordered_map>
#include <vector>

class Database {
public:
  Database(std::vector<CommandPtr> commands);
  BusResponse ExecuteBusQuery(const BusQuery &query);

private:
  std::unordered_map<int, Route> _routes;
};