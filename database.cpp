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
    } else if (query->Kind() == Queries::RouteQuery) {
      res.push_back(ExecuteRouteQuery(static_cast<const RouteQuery &>(*query)));
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
    response.num_stops = route.StopsCount();
    response.num_unique_stops = route.UniqueStopsCount();
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

void Database::BuildMap() {
  MapStorageBuilder builder;
  for (const auto &bus_id : _bus.GetBuses()) {
    BusAndRouteInfo info;
    const auto &route = _route.Get(bus_id);
    info.circular = route.IsCircular();
    info.average_velocity = _settings.bus_velocity;
    info.average_wait_time = _settings.bus_wait_time;

    const auto &unique_stops = route.UniqueStops();
    info.stops = {unique_stops.begin(), unique_stops.end()}; // TODO or keep the set?

    route.IterateByPair([&info, this](StopId first, StopId second) {
      info.distances.push_back({first, second, _given_dist.GetDistance(first, second)});
    });
    builder.AddRouteInfo(info);
  }
  _map.emplace(builder.Build());
}

ResponsePtr Database::ExecuteRouteQuery(const RouteQuery &query) {
  const auto &from = _stop.TryFindByName(query.GetFrom());
  const auto &to = _stop.TryFindByName(query.GetTo());
  if (from != nullptr && to != nullptr) {

    return make_shared<FoundRouteResponse>(query.GetId());
  } else {
    return make_shared<NoRouteResponse>(query.GetId());
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