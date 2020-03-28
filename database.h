#pragma once

#include "bus_storage.h"
#include "command.h"
#include "distance.h"
#include "distance_calc.h"
#include "geomath.h"
#include "query.h"
#include "response.h"
#include "route.h"
#include "route_storage.h"
#include "stop.h"
#include "stop_storage.h"

#include <memory>
#include <unordered_map>
#include <vector>

class DistanceStorage {
public:
  void AddDistances(StopPtr stop,
                    const std::unordered_map<std::string, double> &distances);
  void AddDistance(const StopPair &route, Distance distance);

private:
  std::unordered_map<StopPair, Distance, StopPairHasher> _distances;
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
                                        const std::string &second) const;

private:
  void AddDistances(StopId stop_id,
                    const std::unordered_map<std::string, double> &distances);
  void AddDistance(const StopPair &route, Distance distance);

  BusStorage _bus;
  RouteStorage _route;
  StopStorage _stop;
  // DistanceStorage _distance;

  std::unordered_map<StopPair, Distance, StopPairHasher> _distances;

  HelicopterDistanceCalculator _helicopter_dist{Planet::Earth, _stop};
  GivenDistanceCalculator _given_dist{&_distances};
};