#pragma once

#include "graph.h"
#include "id.h"

#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <vector>

// class MapStop {
// public:
//   MapStop(StopId stop_id) : _stop_id(stop_id) {}
//   StopId GetStopId() const { return _stop_id; }
//   virtual bool IsWait() const = 0;

// private:
//   StopId _stop_id;
// };

// class WaitStop final : public MapStop {
// public:
//   using MapStop::MapStop;
//   virtual bool IsWait() const override { return true; };
// };

// class BusStop final : public MapStop {
// public:
//   BusStop(BusId bus_id, StopId stop_id) : MapStop(stop_id), _bus_id(bus_id) {}
//   virtual bool IsWait() const override { return false; };

// private:
//   BusId _bus_id;
// };

struct BusAndRouteInfo {
  BusId bus_id;
  bool circular;
  double average_velocity;
  double average_wait_time;
  std::vector<StopId> stops;
  std::vector<std::tuple<StopId, StopId, double>> distances;
};

class MapStorageBuilder;

class MapStorage {
  friend class MapStorageBuilder;

public:
  const Graph::DirectedWeightedGraph<double> &Graph() const { return _graph; }

private:
  MapStorage(const Graph::DirectedWeightedGraph<double> &graph) : _graph(std::move(graph)) {}
  const Graph::DirectedWeightedGraph<double> _graph;
};

class MapStorageBuilder {
public:
  MapStorageBuilder() {}
  void AddRouteInfo(const BusAndRouteInfo &info);
  MapStorage Build();

private:
  Graph::VertexId AddOrGetWaitStop(StopId stop_id);
  Graph::VertexId AddBusStop(BusId bus_id, StopId stop_id);
  Graph::VertexId GetWaitStop(StopId stop_id);
  Graph::VertexId GetBusStop(BusId bus_id, StopId stop_id);
  void AddEdgeTemp(Graph::VertexId first, Graph::VertexId second, double time);
  void AddEdge(Graph::VertexId first, Graph::VertexId second, double time);
  size_t TotalStopCount();

  Graph::VertexId _last_vertex_id = 0;

  // WAIT stops are identified by stop id only
  std::unordered_map<Graph::VertexId, StopId> _wait_stops_by_vertices; // index is VertexId, value is StopId
  std::unordered_map<StopId, Graph::VertexId> _vertices_by_wait_stops; // index is StopId, value is VertexId

  // BUS stops are identified by bus and stop ids
  std::unordered_map<Graph::VertexId, std::pair<BusId, StopId>> _bus_stops_by_vertices; // index is VertexId, value is [BusId, StopId]
  std::unordered_map<std::pair<BusId, StopId>, Graph::VertexId> _vertices_by_bus_stops; // index is [BusId, StopId], value is VertexId

  // temporary storage for edges, as we can't store them into graph directly
  std::unordered_map<std::pair<Graph::VertexId, Graph::VertexId>, double> _temp_edges;

  std::unordered_map<Graph::EdgeId, Graph::Edge<double>> _edges;

  // graph has no default constructor, so wrap it in optional
  std::optional<Graph::DirectedWeightedGraph<double>> _graph;
};
