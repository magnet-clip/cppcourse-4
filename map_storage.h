#pragma once

#include "id.h"
#include "map_stop.h"

#include <memory>
#include <optional>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct BusAndRouteInfo {
  BusId bus_id;
  bool circular;
  double average_velocity;
  std::vector<StopId> stops;
  std::vector<std::tuple<StopId, StopId, double>> distances;
};

using VertexIdSet = std::unordered_set<VertexId>;

class MapStorage {
public:
  struct VertexInfo {
    VertexId vertex_id;
    double edge_weight;
  };

  struct VertexRecord {
    std::vector<VertexInfo> items;
    bool visited;
    double total_weight;
  };

  struct EdgeInfo {
    VertexId from;
    VertexId to;
    double weight;
  };

  MapStorage(size_t stops_count) : _stops_by_vertices(4 * stops_count), _incidents(4 * stops_count) {}
  void AddRouteInfo(const BusAndRouteInfo &info);

  void BuildRouter(double average_wait_time);

  std::optional<std::vector<std::tuple<VertexId, VertexId, double>>> FindRoute(VertexId from, VertexId to);

  VertexId GetWaitStop(StopId stop_id) const;
  size_t TotalStopCount() const;
  std::shared_ptr<MapStop> GetStopByVertex(VertexId vertex_id) const;

  const std::unordered_map<StopId, std::unordered_map<BusId, VertexIdSet>> &GetVerticesByBusStops() const {
    return _vertices_by_bus_stops;
  }
  const std::unordered_map<StopId, VertexId> &GetVerticesByWaitStops() const {
    return _vertices_by_wait_stops;
  }

  const std::vector<std::vector<VertexInfo>> &GetIncidents() const { return _incidents; }

private:
  void ResetRoutes();

  VertexId AddOrGetWaitStop(StopId stop_id);
  VertexId AddBusStop(BusId bus_id, StopId stop_id);

  // VertexId -> MapStop
  std::vector<MapStopPtr> _stops_by_vertices;

  // StopId -> VertexId (WAIT stops)
  std::unordered_map<StopId, VertexId> _vertices_by_wait_stops;

  // StopId -> BusId -> { VertexId } (BUS stops)
  std::unordered_map<StopId, std::unordered_map<BusId, VertexIdSet>>
      _vertices_by_bus_stops;

  std::vector<VertexRecord> _incidents;
};

bool operator<(const MapStorage::VertexInfo &a, const MapStorage::VertexInfo &b);