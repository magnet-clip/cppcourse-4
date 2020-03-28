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
  const auto &bus_name = command.GetName();
  _route.Add(bus_name, command.IsCircular(), command.GetStops());
  for (const auto &stop_name : command.GetStops()) {
    const auto &stop = _stop.GetOrAddUnqualified(stop_name);
    stop->AddBus(bus_name);
  }
}

void Database::ExecuteStopCommand(const StopCommand &command) {
  const auto &stop_name = command.GetName();
  const auto &stop_ptr =
      _stop.AddQualifiedStop(stop_name, command.GetLocation());
  for (const auto &bus_name : _route.GetBusesByStop(stop_name)) {
    stop_ptr->AddBus(bus_name);
  }
  AddDistances(stop_ptr, command.GetDistances());
}

void Database::AddDistance(const StopPair &route, Distance distance) {
  if (_distances.count(route)) {
    auto &existing = _distances.at(route);
    if (!distance.implicit && existing.implicit) {
      existing = distance;
    } else if (!distance.implicit && !existing.implicit) {
      throw domain_error("There's already an explicit distance");
    }
  } else {
    _distances.insert({route, distance});
  }
}

void Database::AddDistances(StopPtr stop,
                            const unordered_map<string, double> &distances) {
  for (const auto &[other_name, distance] : distances) {
    const auto &other = _stop.GetOrAddUnqualified(other_name);
    AddDistance({stop->GetName(), other->GetName()}, {distance, false});
    AddDistance({other->GetName(), stop->GetName()}, {distance, true});
  }
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
  const auto bus_name = query.GetNumber();
  if (const auto &route = _route.TryGet(bus_name); route != nullopt) {
    FoundBusResponse response;
    response.bus_number = bus_name;
    response.num_stops = route->GetStopNames().size();
    response.num_unique_stops = route->UniqueStops().size();
    response.length = _given_dist.Calculate(*route);
    response.curvature = response.length / _helicopter_dist.Calculate(*route);
    return make_shared<FoundBusResponse>(response);
  } else {
    return make_shared<NoBusResponse>(bus_name);
  }
}

ResponsePtr Database::ExecuteStopQuery(const StopQuery &query) {
  auto stop_name = query.GetName();
  if (const auto &stop = _stop.TryGet(stop_name); stop != nullptr) {
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
  const auto first_ptr = _stop.TryGet(first);
  const auto second_ptr = _stop.TryGet(second);
  if (first_ptr == nullptr || second_ptr == nullptr) {
    return nullopt;
  }

  return _distances.at({first_ptr->GetName(), second_ptr->GetName()});
}