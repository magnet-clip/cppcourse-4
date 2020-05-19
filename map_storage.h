#pragma once

#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "graph.h"
#include "id.h"
#include "map_stop.h"
#include "router.h"

struct BusAndRouteInfo {
  BusId bus_id;
  bool circular;
  double average_velocity;
  std::vector<StopId> stops;
  std::vector<std::tuple<StopId, StopId, double>> distances;
};

using VertexIdSet = std::unordered_set<Graph::VertexId>;

class MapStorage {
  using VertexId = Graph::VertexId;

 public:
  void AddRouteInfo(const BusAndRouteInfo &info);
  void BuildRouter(double average_wait_time);

  VertexId GetWaitStop(StopId stop_id) const;
  size_t TotalStopCount() const;
  std::shared_ptr<MapStop> GetStopByVertex(VertexId vertex_id) const;

  const std::unordered_map<StopId, std::unordered_map<BusId, VertexIdSet>> &GetVerticesByBusStops() const {
    return _vertices_by_bus_stops;
  }
  const std::unordered_map<StopId, VertexId> &GetVerticesByWaitStops() const {
    return _vertices_by_wait_stops;
  }
  const Graph::Router &GetRouter() const { return _router.value(); }
  const Graph::DirectedWeightedGraph &GetGraph() const {
    return _graph.value();
  }

 private:
  VertexId AddOrGetWaitStop(StopId stop_id);
  VertexId AddBusStop(BusId bus_id, StopId stop_id);

  std::optional<Graph::Router> _router;
  std::optional<Graph::DirectedWeightedGraph> _graph;

  // VertexId -> MapStop
  std::vector<MapStopPtr> _stops_by_vertices;

  // StopId -> VertexId (WAIT stops)
  std::unordered_map<StopId, VertexId> _vertices_by_wait_stops;

  // StopId -> BusId -> { VertexId } (BUS stops)
  std::unordered_map<StopId, std::unordered_map<BusId, VertexIdSet>>
      _vertices_by_bus_stops;

  std::vector<std::tuple<VertexId, VertexId, double>> _temp_edges;
};
