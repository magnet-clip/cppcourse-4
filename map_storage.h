#pragma once

#include "graph.h"
#include "id.h"
#include "map_stop.h"
#include "router.h"

#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <vector>

struct BusAndRouteInfo {
  BusId bus_id;
  bool circular;
  double average_velocity;
  double average_wait_time;
  std::vector<StopId> stops;
  std::vector<std::tuple<StopId, StopId, double>> distances;
};

class MapStorage {
public:
  void AddRouteInfo(const BusAndRouteInfo &info);
  void BuildRouter();
  const Graph::Router<double> &GetRouter() const { return _router.value(); }
  const Graph::DirectedWeightedGraph<double> &GetGraph() const { return _graph.value(); }
  Graph::VertexId GetWaitStop(StopId stop_id) const;
  Graph::VertexId GetBusStop(BusId bus_id, StopId stop_id) const;
  size_t TotalStopCount() const;
  std::shared_ptr<MapStop> GetStopByVertex(Graph::VertexId vertex_id) const;

private:
  Graph::VertexId AddOrGetWaitStop(StopId stop_id);
  Graph::VertexId AddBusStop(BusId bus_id, StopId stop_id);

  std::optional<Graph::Router<double>> _router;
  std::optional<Graph::DirectedWeightedGraph<double>> _graph;

  std::vector<std::shared_ptr<MapStop>> _stops_by_vertices;                                      // VertexId -> MapStop
  std::unordered_map<StopId, Graph::VertexId> _vertices_by_wait_stops;                           // StopId -> VertexId
  std::unordered_map<BusId, std::unordered_map<StopId, Graph::VertexId>> _vertices_by_bus_stops; // BusId -> StopId -> VertexId

  std::vector<std::tuple<Graph::VertexId, Graph::VertexId, double>> _temp_edges;
};
