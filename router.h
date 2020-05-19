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
  Router(const Graph &graph) : graph_(graph) {
    _prev.assign(graph.GetVertexCount(), std::nullopt);
  }

  using RouteId = uint64_t;

  std::optional<std::vector<VertexId>> BuildRoute(VertexId from, VertexId to) const;
  std::optional<double> FindDistance(VertexId from, VertexId to) const;

 private:
  const Graph &graph_;
  mutable std::vector<std::optional<VertexId>> _prev;
};

}  // namespace Graph
