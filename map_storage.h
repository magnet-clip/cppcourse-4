#pragma once

#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "id.h"
#include "map_stop.h"

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
  struct Neighbour {
    VertexId vertex_id;
    double distance;
  };

  struct EdgeInfo {
    VertexId from;
    VertexId to;
    double weight;
  };

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

  const std::unordered_map<VertexId, std::vector<Neighbour>> &GetIncidents() const { return _incidents; }

 private:
  VertexId AddOrGetWaitStop(StopId stop_id);
  VertexId AddBusStop(BusId bus_id, StopId stop_id);
  void AddIncident(VertexId where, VertexId to, double distance);

  // VertexId -> MapStop
  std::vector<MapStopPtr> _stops_by_vertices;

  // StopId -> VertexId (WAIT stops)
  std::unordered_map<StopId, VertexId> _vertices_by_wait_stops;

  // StopId -> BusId -> { VertexId } (BUS stops)
  std::unordered_map<StopId, std::unordered_map<BusId, VertexIdSet>>
      _vertices_by_bus_stops;

  std::unordered_map<VertexId, std::vector<Neighbour>> _incidents;
};