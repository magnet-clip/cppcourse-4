#include "map_storage.h"

using namespace std;
using namespace Graph;

void MapStorageBuilder::AddRouteInfo(const BusAndRouteInfo &info) {
  for (StopId stop_id : info.stops) {
    // Create WAIT stops for each stop
    VertexId wait_vertex_id = _map_stops.AddOrGetWaitStop(stop_id);
    // Create BUS stops for each stop on this route
    VertexId bus_stop_vertex_id = _map_stops.AddBusStop(info.bus_id, stop_id);
    // Add paths between BUS and WAIT stops with appropriate time
    AddEdgeTemp(wait_vertex_id, bus_stop_vertex_id, info.average_wait_time);
    AddEdgeTemp(bus_stop_vertex_id, wait_vertex_id, 0);
  }

  // Add paths between adjacent ribs
  for (size_t i = 0; i < info.distances.size(); i++) {
    const auto &[first_stop_id, next_stop_id, distance] = info.distances[i];
    VertexId first_vertex_id = _map_stops.GetBusStop(first_stop_id, info.bus_id);
    VertexId next_vertex_id;
    if (i == info.distances.size() - 1) {
      next_vertex_id = _map_stops.GetWaitStop(next_stop_id);
    } else {
      next_vertex_id = _map_stops.GetBusStop(next_stop_id, info.bus_id);
    }

    // Calculate times between BUS/BUS stops
    AddEdgeTemp(first_vertex_id, next_vertex_id, distance / info.average_velocity);
  }
}

MapStorage MapStorageBuilder::Build() {
  _graph = DirectedWeightedGraph<double>(_map_stops.TotalStopCount());
  for (const auto &[first, second, time] : _temp_edges) {
    AddEdge(first, second, time);
  }

  return {*_graph, _map_stops};
}

VertexId MapStopsStorage::AddOrGetWaitStop(StopId stop_id) {
  auto it = _vertices_by_wait_stops.find(stop_id);
  if (it == _vertices_by_wait_stops.end()) {
    auto vertex_id = _stops_by_vertices.size();
    _vertices_by_wait_stops.insert({stop_id, vertex_id});
    _stops_by_vertices.push_back(make_shared<WaitStop>(vertex_id, stop_id));
    return vertex_id;
  } else {
    return it->second;
  }
}

VertexId MapStopsStorage::AddBusStop(BusId bus_id, StopId stop_id) {
  auto it = _vertices_by_bus_stops.find(bus_id);
  if (it != _vertices_by_bus_stops.end()) {
    auto insert_result = _vertices_by_bus_stops.insert({bus_id, {}});
    it = insert_result.first;
  }

  auto vertex_id = _stops_by_vertices.size();
  it->second.insert({stop_id, vertex_id});
  _stops_by_vertices.push_back(make_shared<BusStop>(vertex_id, bus_id, stop_id));

  return vertex_id;
}

VertexId MapStopsStorage::GetWaitStop(StopId stop_id) const {
  return _vertices_by_wait_stops.at(stop_id);
}

VertexId MapStopsStorage::GetBusStop(BusId bus_id, StopId stop_id) const {
  return _vertices_by_bus_stops.at(bus_id).at(stop_id);
}

size_t MapStopsStorage::TotalStopCount() const {
  return _stops_by_vertices.size();
}

void MapStorageBuilder::AddEdgeTemp(VertexId first, VertexId second, double time) {
  _temp_edges.push_back({first, second, time});
}

void MapStorageBuilder::AddEdge(VertexId first, VertexId second, double time) {
  Edge<double> edge{first, second, time};
  auto edge_id = _graph->AddEdge(edge);
  _map_stops.AddRoute(edge_id, first, second);
}
