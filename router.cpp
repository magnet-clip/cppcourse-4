#include <router.h>

#include <iostream>
#include <queue>
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
  priority_queue<HalfEdge, vector<HalfEdge>, HalfEdgeComparer> edge_queue;

  vector<double> dist(graph_.GetVertexCount(), INF);
  vector<optional<VertexId>> prev(graph_.GetVertexCount(), nullopt);

  edge_queue.push({from, 0.0});
  dist[from] = 0.0;

  while (!edge_queue.empty()) {
    VertexId u = edge_queue.top().vertex_id;
    edge_queue.pop();

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

optional<Router::Route> Router::BuildRoute(VertexId from, VertexId to) const {
  cout << "Looking for distance between " << from << " and " << to << endl;
  if (_routes[from][to].state == TripleState::NoPath) {
    cout << " - already tried, no route" << endl;
    return nullopt;
  } else if (_routes[from][to].state == TripleState::Exists) {
    cout << " - already tried, there's a route" << endl;
    vector<VertexId> path;

    auto current = to;
    do {
      path.push_back(current);
      current = *_routes[from][current].prev;
    } while (current != from);
    path.push_back(from);

    return {{_routes[from][to].distance, {path.rbegin(), path.rend()}}};
  }
  cout << " - have not tried, will look for a route" << endl;

  // state is Unknown
  priority_queue<HalfEdge, vector<HalfEdge>, HalfEdgeComparer> edge_queue;

  vector<double> dist(graph_.GetVertexCount(), INF);
  vector<optional<VertexId>> prev(graph_.GetVertexCount(), nullopt);

  edge_queue.push({from, 0.0});
  dist[from] = 0.0;

  while (!edge_queue.empty()) {
    VertexId u = edge_queue.top().vertex_id;
    edge_queue.pop();

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

  if (prev[to]) {
    cout << " - found a route" << endl;
    vector<VertexId> path;

    auto current = to;
    do {
      _routes[from][current].state = TripleState::Exists;
      _routes[from][current].distance = dist[current];
      _routes[from][current].prev = prev[current];

      path.push_back(current);
      current = prev[current].value();
    } while (current != from);
    path.push_back(from);

    return {{_routes[from][to].distance, {path.rbegin(), path.rend()}}};
  } else {
    cout << " - no route!" << endl;
    _routes[from][to].state = TripleState::NoPath;
    return nullopt;
  }
}
