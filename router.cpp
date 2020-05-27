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
  if (!_routes[from][to].prev) {
    return nullopt;
  } else {
    return {_routes[from][to].distance};
  }
}

void Router::BuildAllRoutes() const {
  const auto vertex_count = graph_.GetVertexCount();
  for (VertexId from = 0; from < vertex_count; from++) {
    auto& route = _routes[from];
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
  }
}

optional<Router::Route> Router::BuildRoute(VertexId from, VertexId to) const {
  if (!_routes[from][to].prev) {
    return nullopt;
  } else {
    vector<VertexId> path;
    auto current = to;
    do {
      path.push_back(current);
      current = *_routes[from][current].prev;
    } while (current != from);
    path.push_back(from);

    return {{_routes[from][to].distance, {path.rbegin(), path.rend()}}};
  }
}