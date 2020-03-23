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
  // cout << command.ToString() << endl;
  const auto &bus_name = command.GetName();
  _routes.insert({bus_name, {command.IsCircular(), command.GetStops()}});
  for (const auto &stop : command.GetStops()) {
    if (!_stops.count(stop)) {
      _stops.insert({stop, make_shared<Stop>(stop)});
    }
    _stops[stop]->AddBus(bus_name);
  }
}

void Database::ExecuteStopCommand(const StopCommand &command) {
  const auto &stop_name = command.GetName();
  const auto stop_ptr =
      make_shared<QualifiedStop>(stop_name, command.GetLocation());
  _stops[stop_name] = stop_ptr;

  for (const auto &[bus_name, route] : _routes) {
    if (route.UniqueStops().count(stop_name)) {
      stop_ptr->AddBus(bus_name);
    }
  }
  AddDistances(stop_ptr, command.GetDistances());
}

void Database::AddDistances(StopPtr stop,
                            const unordered_map<string, double> &distances) {
  for (const auto &[other_name, distance] : distances) {
    if (!_stops.count(other_name)) {
      _stops.insert({other_name, make_shared<Stop>(other_name)});
    }
    const auto &other = _stops[other_name];
    _distances[{stop->GetName(), other->GetName()}] = distance;
  }
}

double Database::CalculateRealLength(const Route &route) {
  double res = 0.0;

  optional<string> prev = nullopt;
  optional<string> current = nullopt;
  for (const auto next_stop_name : route.GetStopNames()) {
    if (prev && current) {
      res += _distances[{*prev, *current}];
    }
    prev = current;
    current = next_stop_name;
  }
  res += _distances[{*prev, *current}];
  return res;
}

double Database::CalculateHelicopterLength(const Route &route,
                                           const Planet &planet) {
  double res = 0.0;

  GeoPoint const *prev = nullptr;
  GeoPoint const *current = nullptr;
  for (const auto next_stop_name : route.GetStopNames()) {
    if (prev != nullptr && current != nullptr) {
      res += planet.Distance(*prev, *current);
    }
    prev = current;
    current = &(_stops.at(next_stop_name)->GetLocation());
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
    response.length = CalculateRealLength(route);
    response.curvature =
        response.length / CalculateHelicopterLength(route, Planet::Earth);

    return make_shared<FoundBusResponse>(response);
  } else {
    return make_shared<NoBusResponse>(number);
  }
}

ResponsePtr Database::ExecuteStopQuery(const StopQuery &query) {
  auto stop_name = query.GetName();
  if (_stops.count(stop_name)) {
    const auto &stop = _stops.at(stop_name);

    FoundStopResponse response;
    response.stop_name = stop->GetName();
    response.bus_names = stop->GetUniqueBusNames();
    return make_shared<FoundStopResponse>(response);
  } else {
    return make_shared<NoStopResponse>(stop_name);
  }
}

optional<double> Database::GetStopDistance(const string &first,
                                           const string &second) {
  if (!_stops.count(first) || !_stops.count(second)) {
    return nullopt;
  }

  return _distances.at(
      {_stops.at(first)->GetName(), _stops.at(second)->GetName()});
}