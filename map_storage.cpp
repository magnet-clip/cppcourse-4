#include "map_storage.h"

#include <iostream>

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
}

void PrintIncidents(const unordered_map<VertexId, vector<MapStorage::Neighbour>> &incidents) {
  for (const auto &[id, items] : incidents) {
    cout << "Item " << id << " has " << items.size() << " incidents" << endl;
    for (const auto &n : items) {
      cout << "  n " << n.vertex_id << ", distance " << n.distance << endl;
    }
  }
}

optional<vector<tuple<VertexId, VertexId, double>>> MapStorage::FindRoute(VertexId from, VertexId to) {
  cout << "Routing from " << from << " to " << to << endl;

  VertexId u_id = from;
  vector<VertexId> prev(_incidents.size(), to);
  PriorityQueue<std::greater<double>> queue(_incidents.size());

  cout << "There's " << _incidents.size() << " items in incidents list" << endl;
  PrintIncidents(_incidents);

  for (const auto &[id, _] : _incidents) {
    double w = id == from ? 0.0 : VERY_MUCH;
    queue.Insert(id, w);
    cout << "Inserting " << id << " with weight " << w << endl;
  }

  while (queue.Size() > 0) {
    auto u = queue.PopMax();

    u_id = u.item;
    auto u_distance = u.weight;
    if (u_id == to) {
      break;
    }
    cout << "Min vertex id is  " << u_id << ", distance is " << u_distance << ", has " << _incidents[u_id].size() << " incidents" << endl;
    for (auto &neighbor : _incidents[u_id]) {
      auto alt = u_distance + neighbor.distance;

      cout << "--> Neighbor id is  " << neighbor.vertex_id << ", distance is " << neighbor.distance << endl;
      cout << "--> alt weight " << alt << ", current weight is " << queue.GetItem(neighbor.vertex_id).weight << endl;

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
