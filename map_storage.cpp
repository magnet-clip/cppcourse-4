#include "map_storage.h"

using namespace std;
using namespace Graph;

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
    _temp_edges.push_back({prev_vertex_id, next_vertex_id,
                           (distance / info.average_velocity) * (6.0 / 100.0)});
  }
}

void MapStorage::BuildRouter(double average_wait_time) {
  // Add WAIT stops and transitions
  for (const auto &[stop_id, stops_by_bus] : _vertices_by_bus_stops) {
    VertexId wait_vertex_id = AddOrGetWaitStop(stop_id);
    for (const auto &[bus_id, vertex_ids] : stops_by_bus) {
      for (const auto bus_stop_vertex_id : vertex_ids) {
        _temp_edges.push_back(
            {wait_vertex_id, bus_stop_vertex_id, average_wait_time});
        _temp_edges.push_back({bus_stop_vertex_id, wait_vertex_id, 0});
      }
    }
  }

  _graph.emplace(TotalStopCount());
  for (const auto &[first, second, time] : _temp_edges) {
    Edge<double> edge{first, second, time};
    _graph->AddEdge(edge);
  }
  _temp_edges.clear();
  _router.emplace(_graph.value());
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
