#include "map_storage.h"
#include "priority_queue.h"

using namespace std;

static double VERY_MUCH = 1000000000000;

void MapStorage::AddRouteInfo(const BusAndRouteInfo &info) {
  // Add paths between BUS stops
  bool first = true;
  VertexId next_vertex_id, prev_vertex_id;
  for (size_t i = 0; i < info.distances.size(); i++) {
    const auto &[prev_stop_id, next_stop_id, distance] = info.distances[i];

    if (first) {
      first = false;
      prev_vertex_id = AddBusStop(info.bus_id, prev_stop_id);
      next_vertex_id = AddBusStop(info.bus_id, next_stop_id);
    } else {
      prev_vertex_id = next_vertex_id;
      next_vertex_id = AddBusStop(info.bus_id, next_stop_id);
    }

    // Calculate times between BUS/BUS stops
    AddIncident(prev_vertex_id, next_vertex_id, (distance / info.average_velocity) * (6.0 / 100.0));
  }
}

void MapStorage::AddIncident(VertexId where, VertexId to, double distance) {
  _incidents[where].push_back({.vertex_id = to, .distance = distance});
}

void MapStorage::BuildRouter(double average_wait_time) {
  // Add WAIT stops and transitions
  for (const auto &[stop_id, stops_by_bus] : _vertices_by_bus_stops) {
    VertexId wait_vertex_id = AddOrGetWaitStop(stop_id);
    for (const auto &[bus_id, vertex_ids] : stops_by_bus) {
      for (const auto bus_stop_vertex_id : vertex_ids) {
        AddIncident(wait_vertex_id, bus_stop_vertex_id, average_wait_time);
        AddIncident(bus_stop_vertex_id, wait_vertex_id, 0.0);
      }
    }
  }
  _incidents.shrink_to_fit();
}

optional<vector<tuple<VertexId, VertexId, double>>> MapStorage::FindRoute(VertexId from, VertexId to) {
  VertexId u_id = from;
  vector<VertexId> prev(_incidents.size(), to);
  PriorityQueue<VertexId> queue(_incidents.size());

  for (VertexId id = 0; id < _incidents.size(); id++) {
    queue.Insert(id, from ? 0.0 : VERY_MUCH);
  }

  while (u_id != to && queue.Size() > 0) {
    auto u = queue.PopMin();
    auto u_distance = u.weight;
    u_id = u.item;
    for (auto &neighbor : _incidents[u_id]) {
      auto alt = u_distance + neighbor.distance;
      if (alt < queue.GetItem(neighbor.vertex_id).weight) {
        prev[neighbor.vertex_id] = u_id;
        queue.UpdatePriority(neighbor.vertex_id, alt);
      }
    }
  }

  if (u_id != to) {
    return nullopt;
  }

  auto u = to;
  auto prev_u = prev[u];
  vector<tuple<VertexId, VertexId, double>> path;
  do {
    tuple<VertexId, VertexId, double> item{u, prev[u], queue.GetItem(u).weight - queue.GetItem(u).weight};
    path.push_back(item);
    u = prev_u;
    prev_u = prev[u];
  } while (prev_u != from);

  return path;
}

VertexId MapStorage::AddOrGetWaitStop(StopId stop_id) {
  auto it = _vertices_by_wait_stops.find(stop_id);
  if (it != _vertices_by_wait_stops.end()) {
    return it->first;
  } else {
    auto vertex_id = _stops_by_vertices.size();
    _vertices_by_wait_stops.insert({stop_id, vertex_id});
    _stops_by_vertices.push_back(make_shared<WaitStop>(vertex_id, stop_id));
    return vertex_id;
  }
}

VertexId MapStorage::AddBusStop(BusId bus_id, StopId stop_id) {
  auto stop_it = _vertices_by_bus_stops.find(stop_id);
  if (stop_it == _vertices_by_bus_stops.end()) {
    auto insert_result = _vertices_by_bus_stops.insert({stop_id, {}});
    stop_it = insert_result.first;
  }

  auto vertex_id = _stops_by_vertices.size();
  auto &vertex_ids_by_bus = stop_it->second;

  auto bus_it = vertex_ids_by_bus.find(bus_id);
  if (bus_it == vertex_ids_by_bus.end()) {
    auto insert_result = vertex_ids_by_bus.insert({bus_id, {}});
    bus_it = insert_result.first;
  }

  bus_it->second.insert(vertex_id);

  _stops_by_vertices.push_back(
      make_shared<BusStop>(vertex_id, bus_id, stop_id));

  return vertex_id;
}

VertexId MapStorage::GetWaitStop(StopId stop_id) const {
  return _vertices_by_wait_stops.at(stop_id);
}

size_t MapStorage::TotalStopCount() const { return _stops_by_vertices.size(); }

shared_ptr<MapStop> MapStorage::GetStopByVertex(VertexId vertex_id) const {
  return _stops_by_vertices[vertex_id];
}
