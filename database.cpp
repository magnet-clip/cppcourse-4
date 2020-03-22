#include "database.h"

using namespace std;

Database::Database(const vector<CommandPtr> &commands) {
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
  _stops[command.GetName()] = {command.GetLocation(), command.GetName()};
}

double Database::CalculateRouteLength(const Route &route,
                                      const Planet &planet) {
  double res = 0.0;

  GeoPoint *prev = nullptr;
  GeoPoint *current = nullptr;
  for (const auto next_stop : route.GetStopsIterator()) {
    if (prev != nullptr && current != nullptr) {
      res += planet.Distance(*prev, *current);
    }
    prev = current;
    current = &(_stops[next_stop].location);
  }
  return res;
}

unique_ptr<BusResponse> Database::ExecuteBusQuery(const BusQuery &query) {
  auto number = query.GetNumber();
  if (_routes.count(number)) {
    FoundBusResponse response;
    auto &route = _routes.at(number);

    response.bus_number = number;
    response.num_stops = route.StopsCount();
    response.num_unique_stops = route.UniqueStopsCount();
    response.length = CalculateRouteLength(route, Planet::Earth);

    return make_unique<FoundBusResponse>(response);
  } else {
    return make_unique<NoBusResponse>(number);
  }
}