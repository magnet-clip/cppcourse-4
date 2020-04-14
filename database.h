#pragma once

#include "bus_storage.h"
#include "command.h"
#include "distance.h"
#include "distance_calc.h"
#include "distance_storage.h"
#include "geomath.h"
#include "map_storage.h"
#include "query.h"
#include "response.h"
#include "route.h"
#include "route_storage.h"
#include "routing_settings.h"
#include "stop.h"
#include "stop_storage.h"

#include <memory>
#include <vector>

class Database {
public:
  void UseSettings(RoutingSettings settings) { _settings = settings; }
  void ExecuteCommands(const std::vector<CommandPtr> &commands);
  void ExecuteBusCommand(const BusCommand &command);
  void ExecuteStopCommand(const StopCommand &command);

  void BuildMap();

  std::vector<ResponsePtr> ExecuteQueries(const std::vector<QueryPtr> &queries);
  ResponsePtr ExecuteBusQuery(const BusQuery &query);
  ResponsePtr ExecuteStopQuery(const StopQuery &query);
  ResponsePtr ExecuteRouteQuery(const RouteQuery &query);

  std::optional<double> GetStopDistance(const std::string &first,
                                        const std::string &second) const;

private:
  BusStorage _bus;
  RouteStorage _route;
  StopStorage _stop;
  std::optional<MapStorage> _map;
  DistanceStorage _distance;
  RoutingSettings _settings;

  HelicopterDistanceCalculator _helicopter_dist{Planet::Earth, _stop};
  GivenDistanceCalculator _given_dist{_distance};
};