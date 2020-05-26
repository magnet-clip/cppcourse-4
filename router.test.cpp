#include "router.test.h"

#include <utility>

#include "graph.h"
#include "router.h"
#include "test_runner.h"

using namespace std;
using namespace Graph;

void TestRouterSimple() {
  DirectedWeightedGraph g(9);
  g.AddEdge({0, 1, 4});
  g.AddEdge({0, 7, 8});
  g.AddEdge({1, 2, 8});
  g.AddEdge({1, 7, 11});
  g.AddEdge({2, 3, 7});
  g.AddEdge({2, 8, 2});
  g.AddEdge({2, 5, 4});
  g.AddEdge({3, 4, 9});
  g.AddEdge({3, 5, 14});
  g.AddEdge({4, 5, 10});
  g.AddEdge({5, 6, 2});
  g.AddEdge({6, 7, 1});
  g.AddEdge({6, 8, 6});
  g.AddEdge({7, 8, 7});

  const Router r(g);
  {
    auto d = r.FindDistance(0, 0);
    ASSERT_EQUAL(*d, 0);
  }
  {
    auto d = r.FindDistance(0, 1);
    ASSERT_EQUAL(*d, 4);
  }
  {
    auto d = r.FindDistance(0, 2);
    ASSERT_EQUAL(*d, 12);
  }
  {
    auto d = r.FindDistance(0, 3);
    ASSERT_EQUAL(*d, 19);
  }
  {
    auto d = r.FindDistance(0, 4);
    ASSERT_EQUAL(*d, 28);
  }
  {
    auto d = r.FindDistance(0, 5);
    ASSERT_EQUAL(*d, 16);
  }
  {
    auto d = r.FindDistance(0, 6);
    ASSERT_EQUAL(*d, 18);
  }
  {
    auto d = r.FindDistance(0, 7);
    ASSERT_EQUAL(*d, 8);
  }
  {
    auto d = r.FindDistance(0, 8);
    ASSERT_EQUAL(*d, 14);
  }
  {
    auto d = r.FindDistance(7, 0);
    ASSERT(d == nullopt);
  }
}

void TestRouterRoute() {
  DirectedWeightedGraph g(9);
  g.AddEdge({0, 1, 4});
  g.AddEdge({0, 7, 8});
  g.AddEdge({1, 2, 8});
  g.AddEdge({1, 7, 11});
  g.AddEdge({2, 3, 7});
  g.AddEdge({2, 8, 2});
  g.AddEdge({2, 5, 4});
  g.AddEdge({3, 4, 9});
  g.AddEdge({3, 5, 14});
  g.AddEdge({4, 5, 10});
  g.AddEdge({5, 6, 2});
  g.AddEdge({6, 7, 1});
  g.AddEdge({6, 8, 6});
  g.AddEdge({7, 8, 7});

  const Router r(g);
  {
    auto route = r.BuildRoute(0, 0);
    ASSERT(route == nullopt);
  }
  {
    auto route = r.BuildRoute(0, 1);
    ASSERT_EQUAL(route->total_distance, 4);
    ASSERT_EQUAL(route->path.size(), 2UL);
    vector<VertexId> expected_path{0, 1};
    ASSERT_EQUAL(route->path, expected_path);
  }
  {
    auto route = r.BuildRoute(0, 2);
    ASSERT_EQUAL(route->total_distance, 12);
    ASSERT_EQUAL(route->path.size(), 3UL);
    vector<VertexId> expected_path{0, 1, 2};
    ASSERT_EQUAL(route->path, expected_path);
  }
  {
    auto route = r.BuildRoute(0, 3);
    ASSERT_EQUAL(route->total_distance, 19);
    ASSERT_EQUAL(route->path.size(), 4UL);
    vector<VertexId> expected_path{0, 1, 2, 3};
    ASSERT_EQUAL(route->path, expected_path);
  }
  {
    auto route = r.BuildRoute(0, 4);
    ASSERT_EQUAL(route->total_distance, 28);
    ASSERT_EQUAL(route->path.size(), 5UL);
    vector<VertexId> expected_path{0, 1, 2, 3, 4};
    ASSERT_EQUAL(route->path, expected_path);
  }
  {
    auto route = r.BuildRoute(0, 5);
    ASSERT_EQUAL(route->total_distance, 16);
    ASSERT_EQUAL(route->path.size(), 4UL);
    vector<VertexId> expected_path{0, 1, 2, 5};
    ASSERT_EQUAL(route->path, expected_path);
  }
  {
    auto route = r.BuildRoute(0, 6);
    ASSERT_EQUAL(route->total_distance, 18);
    ASSERT_EQUAL(route->path.size(), 5UL);
    vector<VertexId> expected_path{0, 1, 2, 5, 6};
    ASSERT_EQUAL(route->path, expected_path);
  }
  {
    auto route = r.BuildRoute(0, 7);
    ASSERT_EQUAL(route->total_distance, 8);
    ASSERT_EQUAL(route->path.size(), 2UL);
    vector<VertexId> expected_path{0, 7};
    ASSERT_EQUAL(route->path, expected_path);
  }
  {
    auto route = r.BuildRoute(0, 8);
    ASSERT_EQUAL(route->total_distance, 14);
    ASSERT_EQUAL(route->path.size(), 4UL);
    vector<VertexId> expected_path{0, 1, 2, 8};
    ASSERT_EQUAL(route->path, expected_path);
  }
  {
    auto d = r.BuildRoute(7, 0);
    ASSERT(d == nullopt);
  }
}

void TestRouterRoute2() {
  DirectedWeightedGraph g(9);
  g.AddEdge({0, 1, 4});
  g.AddEdge({0, 7, 8});
  g.AddEdge({1, 2, 8});
  g.AddEdge({1, 7, 11});
  g.AddEdge({2, 3, 7});
  g.AddEdge({2, 8, 2});
  g.AddEdge({2, 5, 4});
  g.AddEdge({3, 4, 9});
  g.AddEdge({3, 5, 14});
  g.AddEdge({4, 5, 10});
  g.AddEdge({5, 6, 2});
  g.AddEdge({6, 7, 1});
  g.AddEdge({6, 8, 6});
  g.AddEdge({7, 8, 7});

  const Router r(g);
  auto route = r.BuildRoute(0, 0);
  ASSERT(route == nullopt);

  route = r.BuildRoute(6, 0);
  ASSERT(route == nullopt);

  route = r.BuildRoute(0, 6);
  ASSERT_EQUAL(route->total_distance, 18);
  ASSERT_EQUAL(route->path.size(), 5UL);
  vector<VertexId> expected_path = {0, 1, 2, 5, 6};
  ASSERT_EQUAL(route->path, expected_path);

  route = r.BuildRoute(0, 1);
  ASSERT_EQUAL(route->total_distance, 4);
  ASSERT_EQUAL(route->path.size(), 2UL);
  expected_path = {0, 1};
  ASSERT_EQUAL(route->path, expected_path);

  route = r.BuildRoute(0, 2);
  ASSERT_EQUAL(route->total_distance, 12);
  ASSERT_EQUAL(route->path.size(), 3UL);
  expected_path = {0, 1, 2};
  ASSERT_EQUAL(route->path, expected_path);

  route = r.BuildRoute(0, 3);
  ASSERT_EQUAL(route->total_distance, 19);
  ASSERT_EQUAL(route->path.size(), 4UL);
  expected_path = {0, 1, 2, 3};
  ASSERT_EQUAL(route->path, expected_path);

  route = r.BuildRoute(0, 4);
  ASSERT_EQUAL(route->total_distance, 28);
  ASSERT_EQUAL(route->path.size(), 5UL);
  expected_path = {0, 1, 2, 3, 4};
  ASSERT_EQUAL(route->path, expected_path);

  route = r.BuildRoute(0, 5);
  ASSERT_EQUAL(route->total_distance, 16);
  ASSERT_EQUAL(route->path.size(), 4UL);
  expected_path = {0, 1, 2, 5};
  ASSERT_EQUAL(route->path, expected_path);

  route = r.BuildRoute(0, 6);
  ASSERT_EQUAL(route->total_distance, 18);
  ASSERT_EQUAL(route->path.size(), 5UL);
  expected_path = {0, 1, 2, 5, 6};
  ASSERT_EQUAL(route->path, expected_path);

  route = r.BuildRoute(0, 7);
  ASSERT_EQUAL(route->total_distance, 8);
  ASSERT_EQUAL(route->path.size(), 2UL);
  expected_path = {0, 7};
  ASSERT_EQUAL(route->path, expected_path);

  route = r.BuildRoute(0, 8);
  ASSERT_EQUAL(route->total_distance, 14);
  ASSERT_EQUAL(route->path.size(), 4UL);
  expected_path = {0, 1, 2, 8};
  ASSERT_EQUAL(route->path, expected_path);

  route = r.BuildRoute(7, 0);
  ASSERT(route == nullopt);
}