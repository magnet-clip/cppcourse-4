#pragma once

#include "graph.h"
#include "id.h"
#include "map_stop.h"
#include "router.h"

#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct BusAndRouteInfo {
  BusId bus_id;
  bool circular;
  double average_velocity;
  double average_wait_time;
  std::vector<StopId> stops;
  std::vector<std::tuple<StopId, StopId, double>> distances;
};

using VertexIdSet = std::unordered_set<Graph::VertexId>;

class MapStorage {
  using VertexId = Graph::VertexId;

public:
  void AddRouteInfo(const BusAndRouteInfo &info);
  void BuildRouter();
  const Graph::Router<double> &GetRouter() const { return _router.value(); }
  const Graph::DirectedWeightedGraph<double> &GetGraph() const {
    return _graph.value();
  }
  VertexId GetWaitStop(StopId stop_id) const;
  size_t TotalStopCount() const;
  std::shared_ptr<MapStop> GetStopByVertex(VertexId vertex_id) const;

private:
  VertexId AddWaitStop(StopId stop_id);
  VertexId AddBusStop(BusId bus_id, StopId stop_id);

  std::optional<Graph::Router<double>> _router;
  std::optional<Graph::DirectedWeightedGraph<double>> _graph;

  // VertexId -> MapStop
  std::vector<MapStopPtr> _stops_by_vertices;

  // StopId -> VertexId
  std::unordered_map<StopId, VertexId> _vertices_by_wait_stops;

  // StopId -> BusId -> { VertexId }
  std::unordered_map<StopId, std::unordered_map<BusId, VertexIdSet>>
      _vertices_by_bus_stops;

  std::vector<std::tuple<VertexId, VertexId, double>> _temp_edges;
};
