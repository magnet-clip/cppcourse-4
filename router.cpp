#include <router.h>

#include <iostream>
#include <limits>
#include <queue>
#include <utility>

using namespace std;
using namespace Graph;

struct HalfEdge {
  VertexId vertex_id;
  // optional<EdgeId> edge_id;
  double weight;
};

struct HalfEdgeComparer {
  bool operator()(HalfEdge a, HalfEdge b) {
    return a.weight < b.weight;
  }
};

static const double INF = numeric_limits<double>::max();

optional<double> Router::FindDistance(VertexId from, VertexId to) const {
  priority_queue<HalfEdge, vector<HalfEdge>, HalfEdgeComparer> edge_queue;

  vector<double> dist(graph_.GetVertexCount(), INF);
  vector<optional<VertexId>> prev(graph_.GetVertexCount(), nullopt);

  edge_queue.push({from, 0.0});
  dist[from] = 0.0;

  while (!edge_queue.empty()) {
    VertexId u = edge_queue.top().vertex_id;
    edge_queue.pop();

    // if (u == to) {
    //   break;
    // }

    for (auto incident_edge_id : graph_.GetIncidentEdges(u)) {
      Edge incident_edge = graph_.GetEdge(incident_edge_id);
      VertexId v = incident_edge.to;
      double weight = incident_edge.weight;
      if (dist[v] > dist[u] + weight) {
        dist[v] = dist[u] + weight;
        edge_queue.push({v, dist[v]});
        prev[v] = u;
      }
    }
  }

  if (dist[to] < INF) {
    return dist[to];
  } else {
    return nullopt;
  }
}

optional<Router::RouteInfo2> Router::BuildRoute2(VertexId from, VertexId to) const {
  cout << "Looking for distance between " << from << " and " << to << endl;
  if (from == to) {
    RouteInfo2 res;
    res.path = {from};
    res.total_distance = 0;
    return res;
  }
  priority_queue<HalfEdge, vector<HalfEdge>, HalfEdgeComparer> edge_queue;

  vector<double> dist(graph_.GetVertexCount(), INF);
  vector<VertexId> prev(graph_.GetVertexCount(), from);

  edge_queue.push({from, 0.0});
  dist[from] = 0.0;

  while (!edge_queue.empty()) {
    VertexId u = edge_queue.top().vertex_id;
    edge_queue.pop();

    // if (u == to) break;

    for (auto incident_edge_id : graph_.GetIncidentEdges(u)) {
      Edge incident_edge = graph_.GetEdge(incident_edge_id);
      VertexId v = incident_edge.to;
      double weight = incident_edge.weight;
      if (dist[v] > dist[u] + weight) {
        dist[v] = dist[u] + weight;
        edge_queue.push({v, dist[v]});
        prev[v] = u;
      }
    }
  }

  if (dist[to] < INF) {
    vector<VertexId> path;

    auto current = to;
    do {
      path.push_back(current);
      current = prev[current];
    } while (current != from);
    path.push_back(from);

    Router::RouteInfo2 res;
    res.total_distance = dist[to];
    res.path = {path.rbegin(), path.rend()};

    return res;
  } else {
    return nullopt;
  }
}
