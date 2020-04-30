#include "map_storage.h"

#include <cmath>
#include <queue>

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
    _incidents[prev_vertex_id].items.push_back({.vertex_id = next_vertex_id,
                                                .edge_weight = (distance / info.average_velocity) * (6.0 / 100.0)});
  }
}

void MapStorage::ResetRoutes() {
  _stops_by_vertices.shrink_to_fit();
  _incidents.shrink_to_fit();
  for (auto &record : _incidents) {
    record.total_weight = VERY_MUCH;
    record.visited = false;
  }
}

void MapStorage::BuildRouter(double average_wait_time) {
  // Add WAIT stops and transitions
  for (const auto &[stop_id, stops_by_bus] : _vertices_by_bus_stops) {
    VertexId wait_vertex_id = AddOrGetWaitStop(stop_id);
    for (const auto &[bus_id, vertex_ids] : stops_by_bus) {
      for (const auto bus_stop_vertex_id : vertex_ids) {
        _incidents[wait_vertex_id].items.push_back({.vertex_id = bus_stop_vertex_id,
                                                    .edge_weight = average_wait_time});
        _incidents[bus_stop_vertex_id].items.push_back({.vertex_id = wait_vertex_id,
                                                        .edge_weight = 0.0});
      }
    }
  }
}

optional<vector<tuple<VertexId, VertexId, double>>> MapStorage::FindRoute(VertexId from, VertexId to) {
  ResetRoutes();

  double min_distance = 0.0;

  VertexId current_vertex_id = from;
  vector<VertexInfo *> current_vertex_queue;
  vector<VertexInfo *> next_vertex_queue;

  do {
    for (auto &vertex_info : _incidents[current_vertex_id].items) {
      current_vertex_queue.push_back(&vertex_info);
    }
    // TODO sort current_vertex_queue by total_weight and edge_weight

    for (const auto &item : current_vertex_queue) {
      auto &another_vertex = _incidents[item->vertex_id];
      if (_incidents[item->vertex_id].visited) {
        continue;
      }

      const auto new_weight = min_distance + item->edge_weight;
      if (new_weight < another_vertex.total_weight) {
        another_vertex.total_weight = new_weight;
      }
      next_vertex_queue.push_back(item);
    }
    _incidents[current_vertex_id].visited = true;
    // TODO sort next_vertex_queue by total_weight and edge_weight

    // TODO у меня сейчас есть список next_vertex_queue - все вершины, соседние с данной и еще не просмотренные
    // у каждой из них есть свой список смежности. Если я пойду вглубь по этому списку смежности, то next_vertex_queue
    // будет разрастаться. Не совсем понятно как надо

    // TODO wiki: из ещё не посещённых вершин выбирается вершина u, имеющая минимальную метку. не посещенные с минимальной
    //  меткой это те, которых мы хоть как-то потрогали
    //

  } while (true);

  return nullopt;
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
