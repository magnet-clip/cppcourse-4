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
  auto bus_id = _bus.Add(bus_name);
  vector<StopId> stop_ids = _stop.GetOrAddUnqualifiedBulk(command.GetStops());

  _route.Add(bus_id, command.IsCircular(), stop_ids);
  for (const auto &stop_id : stop_ids) {
    _stop.Get(stop_id)->AddBus(bus_id);
  }
}

void Database::ExecuteStopCommand(const StopCommand &command) {
  const auto &stop_name = command.GetName();
  const auto &stop_id =
      _stop.AddQualifiedStop(stop_name, command.GetLocation());
  const auto stop_ptr = _stop.Get(stop_id);
  for (const auto &bus_id : _route.GetBusesByStop(stop_id)) {
    stop_ptr->AddBus(bus_id);
  }
  vector<pair<StopId, double>> distances;
  for (const auto &[other_name, distance] : command.GetDistances()) {
    distances.push_back({_stop.GetOrAddUnqualified(other_name), distance});
  }
  _distance.AddDistances(stop_id, distances);
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
  const auto bus_name = query.GetName();

  if (auto bus_id = _bus.TryFind(bus_name); bus_id != nullopt) {
    const auto &route = _route.Get(*bus_id);

    FoundBusResponse response(query.GetId());
    response.bus_name = bus_name;
    response.num_stops = route.GetStopIds().size();
    response.num_unique_stops = route.UniqueStops().size();
    response.length = _given_dist.Calculate(route);
    response.curvature = response.length / _helicopter_dist.Calculate(route);

    return make_shared<FoundBusResponse>(response);
  } else {
    return make_shared<NoBusResponse>(query.GetId(), bus_name);
  }
}

ResponsePtr Database::ExecuteStopQuery(const StopQuery &query) {
  auto stop_name = query.GetName();
  if (const auto &stop = _stop.TryFindByName(stop_name); stop != nullptr) {
    FoundStopResponse response(query.GetId());
    response.stop_name = stop->GetName();
    response.bus_names = _bus.GetBusNames(stop->GetUniqueBuses());
    return make_shared<FoundStopResponse>(response);
  } else {
    return make_shared<NoStopResponse>(query.GetId(), stop_name);
  }
}

optional<double> Database::GetStopDistance(const std::string &first,
                                           const std::string &second) const {
  auto first_id = _stop.TryFindIdByName(first);
  auto second_id = _stop.TryFindIdByName(second);

  if (first_id == nullopt || second_id == nullopt) {
    return nullopt;
  }

  return _distance.Get({*first_id, *second_id});
};