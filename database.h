#pragma once

#include "command.h"
#include "query.h"
#include "response.h"
#include "route.h"
#include "stop.h"

#include <memory>
#include <unordered_map>
#include <vector>

class Database {
public:
  Database(const std::vector<CommandPtr> &commands);
  std::unique_ptr<BusResponse> ExecuteBusQuery(const BusQuery &query);
  void ExecuteBusCommand(const BusCommand &command);
  void ExecuteStopCommand(const StopCommand &command);

private:
  std::unordered_map<int, Route> _routes;
  std::unordered_map<std::string, Stop> _stops;
};