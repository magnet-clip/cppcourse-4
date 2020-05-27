#include <router.h>

// #include <iostream>
#include <utility>

using namespace std;
using namespace Graph;

struct HalfEdge {
  VertexId vertex_id;
  double weight;
};

struct HalfEdgeComparer {
  bool operator()(HalfEdge a, HalfEdge b) {
    return a.weight < b.weight;
  }
};

optional<double> Router::FindDistance(VertexId from, VertexId to) const {
  if (from == to) {
    return {0};
  }
  const auto vertex_count = graph_.GetVertexCount();
  std::vector<RouteInfo> route(vertex_count, {INF, std::nullopt});
  route[from].distance = 0;
  auto changed = true;
  while (changed) {
    changed = false;
    for (EdgeId edgeId = 0; edgeId < graph_.GetEdgeCount(); edgeId++) {
      const auto edge = graph_.GetEdge(edgeId);
      if (route[edge.from].distance < INF) {
        const auto new_weight = route[edge.from].distance + edge.weight;
        if (route[edge.to].distance > new_weight) {
          route[edge.to].prev = {edge.from};
          route[edge.to].distance = new_weight;
          changed = true;
        }
      }
    }
  }

  if (!route[to].prev) {
    return nullopt;
  } else {
    return {route[to].distance};
  }
}

optional<Router::Route> Router::BuildRoute(VertexId from, VertexId to) const {
  if (from == to) {
    return {{0, {}}};
  }
  const auto vertex_count = graph_.GetVertexCount();
  std::vector<RouteInfo> route(vertex_count, {INF, std::nullopt});
  route[from].distance = 0;
  auto changed = true;
  while (changed) {
    changed = false;
    for (EdgeId edgeId = 0; edgeId < graph_.GetEdgeCount(); edgeId++) {
      const auto edge = graph_.GetEdge(edgeId);
      if (route[edge.from].distance < INF) {
        const auto new_weight = route[edge.from].distance + edge.weight;
        if (route[edge.to].distance > new_weight) {
          route[edge.to].prev = {edge.from};
          route[edge.to].distance = new_weight;
          changed = true;
        }
      }
    }
  }

  if (!route[to].prev) {
    return nullopt;
  } else {
    vector<VertexId> path;
    auto current = to;
    do {
      path.push_back(current);
      current = *route[current].prev;
    } while (current != from);
    path.push_back(from);

    return {{route[to].distance, {path.rbegin(), path.rend()}}};
  }
}