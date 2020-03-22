#include "database.h"
#include <set>
using namespace std;

void Database::ExecuteCommands(const std::vector<CommandPtr> &commands) {
  for (const auto &command_ptr : commands) {
    if (command_ptr->Kind() == Commands::BusCommand) {
      ExecuteBusCommand(static_cast<const BusCommand &>(*command_ptr));
    } else if (command_ptr->Kind() == Commands::StopCommand) {
      ExecuteStopCommand(static_cast<const StopCommand &>(*command_ptr));
    }
  }
}

void Database::ExecuteBusCommand(const BusCommand &command) {
  Route route(command.IsCircular(), command.GetStops());
  _routes.insert({command.GetNumber(), route});
}

void Database::ExecuteStopCommand(const StopCommand &command) {
  _stops.insert(
      {command.GetName(), {command.GetName(), command.GetLocation()}});
}

double Database::CalculateRouteLength(const Route &route,
                                      const Planet &planet) {
  double res = 0.0;

  GeoPoint const *prev = nullptr;
  GeoPoint const *current = nullptr;
  for (const auto next_stop_name : route.GetStopNames()) {
    if (prev != nullptr && current != nullptr) {
      res += planet.Distance(*prev, *current);
    }
    prev = current;
    current = &(_stops.at(next_stop_name).GetLocation());
  }
  res += planet.Distance(*prev, *current);
  return res;
}

vector<ResponsePtr> Database::ExecuteQueries(const vector<QueryPtr> &queries) {
  vector<ResponsePtr> res;
  for (const auto &query : queries) {
    if (query->Kind() == Queries::BusQuery) {
      res.push_back(ExecuteBusQuery(static_cast<const BusQuery &>(*query)));
    } else if (query->Kind() == Queries::StopQuery) {
      res.push_back(ExecuteStopQuery(static_cast<const StopQuery &>(*query)));
    }
  }
  return res;
}

ResponsePtr Database::ExecuteBusQuery(const BusQuery &query) {
  auto number = query.GetNumber();
  if (_routes.count(number)) {
    FoundBusResponse response;
    auto &route = _routes.at(number);

    response.bus_number = number;
    response.num_stops = route.GetStopNames().size();
    response.num_unique_stops = route.UniqueStops().size();
    response.length = CalculateRouteLength(route, Planet::Earth);

    return make_shared<FoundBusResponse>(response);
  } else {
    return make_shared<NoBusResponse>(number);
  }
}

ResponsePtr Database::ExecuteStopQuery(const StopQuery &query) {
  auto stop_name = query.GetName();
  if (_stops.count(stop_name)) {
    FoundStopResponse response;
    response.stop_number = stop_name;
    set<string> bus_names;
    for (const auto &[bus_name, stops] : _routes) {
      if (stops.UniqueStops().count(stop_name)) {
        bus_names.insert(bus_name);
      }
    }
    response.stops.assign(bus_names.begin(), bus_names.end());
    return make_shared<FoundStopResponse>(response);
  } else {
    return make_shared<NoStopResponse>(stop_name);
  }
}
