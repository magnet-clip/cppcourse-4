#include "graph.h"

using namespace std;
using namespace Graph;

DirectedWeightedGraph::DirectedWeightedGraph(size_t vertex_count) : incidence_lists_(vertex_count) {}

EdgeId DirectedWeightedGraph::AddEdge(const Edge &edge) {
  edges_.push_back(edge);
  const EdgeId id = edges_.size() - 1;
  incidence_lists_[edge.from].push_back(id);
  _distances[{edge.from, edge.to}] = edge.weight;
  return id;
}

size_t DirectedWeightedGraph::GetVertexCount() const {
  return incidence_lists_.size();
}

size_t DirectedWeightedGraph::GetEdgeCount() const {
  return edges_.size();
}

const Edge &DirectedWeightedGraph::GetEdge(EdgeId edge_id) const {
  return edges_[edge_id];
}

typename DirectedWeightedGraph::IncidentEdgesRange
DirectedWeightedGraph::GetIncidentEdges(VertexId vertex) const {
  const auto &edges = incidence_lists_[vertex];
  return {begin(edges), end(edges)};
}