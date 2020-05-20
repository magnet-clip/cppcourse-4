#pragma once

#include <cstdlib>
#include <deque>
#include <unordered_map>
#include <vector>

#include "pair_hasher.h"

template <typename It>
class Range {
 public:
  using ValueType = typename std::iterator_traits<It>::value_type;

  Range(It begin, It end) : begin_(begin), end_(end) {}
  It begin() const { return begin_; }
  It end() const { return end_; }

 private:
  It begin_;
  It end_;
};

namespace Graph {

using VertexId = size_t;
using EdgeId = size_t;

struct Edge {
  VertexId from;
  VertexId to;
  double weight;
};

class DirectedWeightedGraph {
 private:
  using IncidenceList = std::vector<EdgeId>;
  using IncidentEdgesRange = Range<typename IncidenceList::const_iterator>;

 public:
  DirectedWeightedGraph(size_t vertex_count);
  EdgeId AddEdge(const Edge &edge);

  size_t GetVertexCount() const;
  size_t GetEdgeCount() const;
  const Edge &GetEdge(EdgeId edge_id) const;
  IncidentEdgesRange GetIncidentEdges(VertexId vertex) const;

  double GetDistance(VertexId from, VertexId to) const { return _distances.at({from, to}); }

 private:
  std::vector<Edge> edges_;
  std::vector<IncidenceList> incidence_lists_;  // VertexId (from) -> {VertexId (from), VertexId (to), double (weight)} []
  std::unordered_map<std::pair<VertexId, VertexId>, double, PairHasher> _distances;
};

}  // namespace Graph
