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
    auto changed = true;
    while (changed) {
      changed = false;
      for (EdgeId edgeId = 0; edgeId < graph_.GetEdgeCount(); edgeId++) {
        const auto edge = graph_.GetEdge(edgeId);
        auto& route = _routes[from];
        route[edge.from].distance = 0.0;
        if (route[edge.from].distance < INF) {
          const auto new_weight = route[edge.from].distance + edge.weight;
          if (route[edge.to].distance > new_weight) {
            route[edge.to].prev = {edge.to};
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
    double total_distance = 0.0;
    auto current = to;
    do {
      path.push_back(current);
      current = *_routes[from][current].prev;
      total_distance += _routes[from][current].distance;
    } while (current != from);
    path.push_back(from);

    return {{total_distance, {path.rbegin(), path.rend()}}};
  }
}