#include "database.h"

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
  for (const auto &bus_id : _bus.GetBuses()) {
    BusAndRouteInfo info;
    const auto &route = _route.Get(bus_id);
    info.bus_id = bus_id;
    info.circular = route.IsCircular();
    info.average_velocity = _settings.bus_velocity;
    info.stops = route.Stops();

    route.IterateByPair([&info, this](StopId first, StopId second) {
      info.distances.push_back(
          {first, second, _given_dist.GetDistance(first, second)});
    });
    _map.AddRouteInfo(info);
  }
  _map.BuildRouter(_settings.bus_wait_time);
}

ResponsePtr Database::ExecuteRouteQuery(const RouteQuery &query) {
  const auto &from = _stop.TryFindByName(query.GetFrom());
  const auto &to = _stop.TryFindByName(query.GetTo());
  if (from == nullptr || to == nullptr) {
    return make_shared<NoRouteResponse>(query.GetId());
  }

  // const auto &router = _map.GetRouter();

  // 1) Find VertexIds of WAIT_STOPs for from and to
  auto from_wait_stop_vertex_id = _map.GetWaitStop(from->GetId());
  auto to_wait_stop_vertex_id = _map.GetWaitStop(to->GetId());

  // 2) Build route from WAIT_STOP to WAIT_STOP
  const auto &route =
      _map.FindRoute(from_wait_stop_vertex_id, to_wait_stop_vertex_id);

  if (!route) {
    return make_shared<NoRouteResponse>(query.GetId());
  }

  // 3) Trace route by edges and make a response
  FoundRouteResponse response(query.GetId());
  // TODO calculate total time
  // response.total_time = route->weight;

  for (size_t idx = 0; idx < route->size(); idx++) {
    const auto [prev_vertex_id, curr_vertex_id, time] = route->at(idx);
    const auto prev_stop = _map.GetStopByVertex(prev_vertex_id);
    const auto curr_stop = _map.GetStopByVertex(curr_vertex_id);

    if (prev_stop->IsWait() && curr_stop->IsWait()) {
      // Wait Stop -> Wait Stop
      throw domain_error("Two wait stops in row");
    } else if (prev_stop->IsWait() && !curr_stop->IsWait()) {
      // Wait Stop -> Bus Stop
      auto prev_stop_id = prev_stop->GetStopId();
      auto curr_stop_id = curr_stop->GetStopId();
      if (prev_stop_id != curr_stop_id) {
        throw domain_error("Can't move from wait stop to another bus stop");
      }
      auto prev_stop = _stop.Get(prev_stop_id);

      WaitRouteItem wait;
      wait.stop_name = prev_stop->GetName();
      wait.time = time;

      response.items.push_back(make_shared<WaitRouteItem>(wait));
    } else if (!prev_stop->IsWait() && !curr_stop->IsWait()) {
      // Bus Stop -> Bus Stop
      auto prev_stop_id = prev_stop->GetStopId();
      auto curr_stop_id = curr_stop->GetStopId();
      if (prev_stop_id == curr_stop_id) {
        throw domain_error("Should have moved to another stop");
      }

      const auto &prev_bus_stop = static_cast<const BusStop &>(*prev_stop);
      const auto &curr_bus_stop = static_cast<const BusStop &>(*curr_stop);
      if (prev_bus_stop.GetBusId() != curr_bus_stop.GetBusId()) {
        throw domain_error("Can't switch bus between bus stops");
      }

      if (response.items.back()->Kind() == RouteItemType::Bus) {
        auto &last_bus = static_cast<BusRouteItem &>(*response.items.back());
        last_bus.time += time;
        last_bus.span_count += 1;
      } else {
        BusRouteItem bus;
        bus.time = time;
        bus.span_count = 1;
        bus.bus_name = _bus.GetName(curr_bus_stop.GetBusId());

        response.items.push_back(make_shared<BusRouteItem>(bus));
      }
    } else if (!prev_stop->IsWait() && curr_stop->IsWait()) {
      // Bus Stop -> Wait Stop
      if (time != 0) {
        // There can be a case when the time not zero - circular routes
        auto prev_stop_id = prev_stop->GetStopId();
        auto curr_stop_id = curr_stop->GetStopId();
        if (prev_stop_id == curr_stop_id) {
          throw domain_error("Should have moved to another stop");
        }

        const auto &prev_bus_stop = static_cast<const BusStop &>(*prev_stop);

        if (response.items.back()->Kind() == RouteItemType::Bus) {
          auto &last_bus = static_cast<BusRouteItem &>(*response.items.back());
          last_bus.time += time;
          last_bus.span_count += 1;
        } else {
          BusRouteItem bus;
          bus.time = time;
          bus.span_count = 1;
          bus.bus_name = _bus.GetName(prev_bus_stop.GetBusId());

          response.items.push_back(make_shared<BusRouteItem>(bus));
        }
      }
    }
  }
  return make_shared<FoundRouteResponse>(response);
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