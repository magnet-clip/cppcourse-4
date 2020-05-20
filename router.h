#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "graph.h"
#include "pair_hasher.h"

namespace Graph {

const double INF = std::numeric_limits<double>::max();

enum class TripleState {
  Unknown,
  NoPath,
  Exists
};

struct RouteInfo {
  TripleState state;
  double distance;
  std::optional<VertexId> prev;
};

class Router {
 private:
  using Graph = DirectedWeightedGraph;

  struct Route {
    double total_distance;
    std::vector<VertexId> path;
  };

 public:
  Router(const Graph &graph) : graph_(graph) {
    // _routes.reserve(graph.GetVertexCount());
    // for (size_t idx = 0; idx < graph.GetVertexCount(); idx++) {
    //   _routes.push_back({graph.GetVertexCount(), {TripleState::Unknown, INF, std::nullopt}});
    // }

    _routes.assign(graph.GetVertexCount(), {graph.GetVertexCount(), {TripleState::Unknown, INF, std::nullopt}});
  }

  using RouteId = uint64_t;

  std::optional<Route> BuildRoute(VertexId from, VertexId to) const;
  std::optional<double> FindDistance(VertexId from, VertexId to) const;

 private:
  const Graph &graph_;
  // (from: VertexId)  -> (to: VertexId)  -> (prev: VertexId)
  mutable std::vector<std::vector<RouteInfo>> _routes;
};

}  // namespace Graph
