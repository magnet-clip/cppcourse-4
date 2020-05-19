#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "graph.h"

namespace Graph {

class Router {
 private:
  using Graph = DirectedWeightedGraph;

 public:
  Router(const Graph &graph) : graph_(graph) {}

  using RouteId = uint64_t;

  struct RouteInfo {
    RouteId id;
    double weight;
    size_t edge_count;
  };

  struct RouteInfo2 {
    double total_distance;
    std::vector<VertexId> path;
  };

  struct RouteInfo3 {
    double total_distance;
    std::vector<EdgeId> path;
  };

  std::optional<RouteInfo> BuildRoute(VertexId from, VertexId to) const;
  std::optional<RouteInfo2> BuildRoute2(VertexId from, VertexId to) const;
  std::optional<RouteInfo3> BuildRoute3(VertexId from, VertexId to) const;
  std::optional<double> FindDistance(VertexId from, VertexId to) const;

  EdgeId GetRouteEdge(RouteId route_id, size_t edge_idx) const {
    return expanded_routes_cache_.at(route_id)[edge_idx];
  }

  void ReleaseRoute(RouteId route_id) {
    expanded_routes_cache_.erase(route_id);
  }

 private:
  const Graph &graph_;

  using ExpandedRoute = std::vector<EdgeId>;
  mutable RouteId next_route_id_ = 0;
  mutable std::unordered_map<RouteId, ExpandedRoute> expanded_routes_cache_;
};

}  // namespace Graph
