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

struct RouteInfo {
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
  }

  using RouteId = uint64_t;

  std::optional<Route> BuildRoute(VertexId from, VertexId to) const;
  std::optional<double> FindDistance(VertexId from, VertexId to) const;

 private:
  const Graph &graph_;
};

}  // namespace Graph
