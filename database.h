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
  Database() {}

  void ExecuteCommands(const std::vector<CommandPtr> &commands);
  void ExecuteBusCommand(const BusCommand &command);
  void ExecuteStopCommand(const StopCommand &command);

  std::vector<ResponsePtr> ExecuteQueries(const std::vector<QueryPtr> &queries);
  ResponsePtr ExecuteBusQuery(const BusQuery &query);
  ResponsePtr ExecuteStopQuery(const StopQuery &query);

  std::optional<double> GetStopDistance(const std::string &first,
                                        const std::string &second);

private:
  void AddDistances(const Stop &stop,
                    const std::unordered_map<std::string, double> &distances);
  double CalculateRouteLength(const Route &route, const Planet &planet);
  std::unordered_map<std::string, Route> _routes;
  std::unordered_map<std::string, Stop> _stops;
  std::unordered_map<StopPair, double, StopPairHasher> _distances;
};