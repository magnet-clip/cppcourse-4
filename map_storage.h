#pragma once

#include "graph.h"
#include "id.h"
#include "router.h"

#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <vector>

class MapStop {
public:
  MapStop(Graph::VertexId id, StopId stop_id) : _id(id), _stop_id(stop_id) {}
  Graph::VertexId GetId() const { return _id; }
  StopId GetStopId() const { return _stop_id; }
  virtual bool IsWait() const = 0;

private:
  Graph::VertexId _id;
  StopId _stop_id;
};

class WaitStop final : public MapStop {
public:
  using MapStop::MapStop;
  virtual bool IsWait() const override { return true; };
};

class BusStop final : public MapStop {
public:
  BusStop(Graph::VertexId id, BusId bus_id, StopId stop_id) : MapStop(id, stop_id), _bus_id(bus_id) {}
  StopId GetBusId() const { return _bus_id; }
  virtual bool IsWait() const override { return false; };

private:
  BusId _bus_id;
};

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
  Graph::Router<double> GetRouter() const { return _router.value(); }
  Graph::VertexId GetWaitStop(StopId stop_id) const;
  Graph::VertexId GetBusStop(BusId bus_id, StopId stop_id) const;
  size_t TotalStopCount() const;

private:
  Graph::VertexId AddOrGetWaitStop(StopId stop_id);
  Graph::VertexId AddBusStop(BusId bus_id, StopId stop_id);
  void AddRoute(Graph::EdgeId edge_id, Graph::VertexId first_id, Graph::VertexId second_id);

  std::optional<Graph::Router<double>> _router;
  std::optional<Graph::DirectedWeightedGraph<double>> _graph;

  std::vector<std::shared_ptr<MapStop>> _stops_by_vertices;                                      // VertexId -> MapStop
  std::unordered_map<StopId, Graph::VertexId> _vertices_by_wait_stops;                           // StopId -> VertexId
  std::unordered_map<BusId, std::unordered_map<StopId, Graph::VertexId>> _vertices_by_bus_stops; // BusId -> StopId -> VertexId
  std::unordered_map<Graph::EdgeId, std::pair<Graph::VertexId, Graph::VertexId>> _stops_by_edge; // EdgeId -> [VertexId, VertexId]

  std::vector<std::tuple<Graph::VertexId, Graph::VertexId, double>> _temp_edges;
};
