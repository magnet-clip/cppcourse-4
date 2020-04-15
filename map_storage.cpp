#include "map_storage.h"

using namespace std;
using namespace Graph;

void MapStorageBuilder::AddRouteInfo(const BusAndRouteInfo &info) {
  for (StopId stop_id : info.stops) {
    // Create WAIT stops for each stop
    VertexId wait_vertex_id = AddOrGetWaitStop(stop_id);
    // Create BUS stops for each stop on this route
    VertexId bus_stop_vertex_id = AddBusStop(info.bus_id, stop_id);
    // Add paths between BUS and WAIT stops with appropriate time
    AddEdgeTemp(wait_vertex_id, bus_stop_vertex_id, info.average_wait_time);
    AddEdgeTemp(bus_stop_vertex_id, wait_vertex_id, 0);
  }

  // Add paths between adjacent ribs
  bool first = true;
  for (size_t i = 0; i < info.distances.size(); i++) {
    const auto &[first_stop_id, next_stop_id, distance] = info.distances[i];
    VertexId first_vertex_id = GetBusStop(first_stop_id, info.bus_id);
    VertexId next_vertex_id;
    if (i == info.distances.size() - 1) {
      next_vertex_id = GetWaitStop(next_stop_id);
    } else {
      next_vertex_id = GetBusStop(next_stop_id, info.bus_id);
    }

    // Calculate times between BUS/BUS stops
    AddEdgeTemp(first_vertex_id, next_vertex_id, distance / info.average_velocity);
  }
}

MapStorage MapStorageBuilder::Build() {
  _graph = DirectedWeightedGraph<double>(TotalStopCount());
  for (const auto &[vertices, time] : _temp_edges) {
    AddEdge(vertices.first, vertices.second, time);
  }
  _temp_edges.clear();

  return {*_graph};
}

VertexId MapStorageBuilder::AddOrGetWaitStop(StopId stop_id) {
  auto it = _vertices_by_wait_stops.find(stop_id);
  if (it == _vertices_by_wait_stops.end()) {
    auto vertex_id = _last_vertex_id++;
    _vertices_by_wait_stops.insert({stop_id, vertex_id});
    _wait_stops_by_vertices.insert({vertex_id, stop_id});
    return vertex_id;
  } else {
    return it->second;
  }
}

VertexId MapStorageBuilder::AddBusStop(BusId bus_id, StopId stop_id) {
  auto vertex_id = _last_vertex_id++;
  _vertices_by_bus_stops.insert({{bus_id, stop_id}, vertex_id});
  _bus_stops_by_vertices.insert({vertex_id, {bus_id, stop_id}});
  return vertex_id;
}

VertexId MapStorageBuilder::GetWaitStop(StopId stop_id) {
  return _vertices_by_wait_stops[stop_id];
}

VertexId MapStorageBuilder::GetBusStop(BusId bus_id, StopId stop_id) {
  return _vertices_by_bus_stops[{bus_id, stop_id}];
}

void MapStorageBuilder::AddEdgeTemp(VertexId first, VertexId second, double time) {
  _temp_edges.insert({{first, second}, time});
}

void MapStorageBuilder::AddEdge(VertexId first, VertexId second, double time) {
  // todo save EdgeId
  Edge<double> edge{first, second, time};
  auto edge_id = _graph->AddEdge(edge);
}

size_t MapStorageBuilder::TotalStopCount() {
  return _bus_stops_by_vertices.size() + _wait_stops_by_vertices.size();
}