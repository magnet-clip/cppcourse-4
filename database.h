#pragma once

#include "command.h"
#include "query.h"
#include "response.h"
#include "route.h"
#include "stop.h"

#include <memory>
#include <unordered_map>
#include <vector>

struct Distance {
  double distance;
  bool implicit;
  operator double() { return distance; }
};

class Database {
public:
  void ExecuteCommands(const std::vector<CommandPtr> &commands);
  void ExecuteBusCommand(const BusCommand &command);
  void ExecuteStopCommand(const StopCommand &command);

  std::vector<ResponsePtr> ExecuteQueries(const std::vector<QueryPtr> &queries);
  ResponsePtr ExecuteBusQuery(const BusQuery &query);
  ResponsePtr ExecuteStopQuery(const StopQuery &query);

  std::optional<double> GetStopDistance(const std::string &first,
                                        const std::string &second);

private:
  void AddDistances(StopPtr stop,
                    const std::unordered_map<std::string, double> &distances);
  void AddDistance(const StopPair &route, Distance distance);
  double CalculateHelicopterLength(const Route &route, const Planet &planet);
  double CalculateRealLength(const Route &route);

  std::unordered_map<std::string, Route> _routes;
  std::unordered_map<std::string, StopPtr> _stops;
  std::unordered_map<StopPair, Distance, StopPairHasher> _distances;
};