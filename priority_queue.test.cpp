#include "priority_queue.test.h"
#include "priority_queue.h"
#include "test_runner.h"

#include <vector>

using namespace std;

void TestVV() {
  vector<vector<int>> x;
  x.resize(61);
  x[5].push_back(1);
}

void TestCreateQueue() {
  PriorityQueue<int> queue(5);
  ASSERT_EQUAL(queue.Size(), 0UL);
}

void TestGetItemsQueue() {
  vector<QueueItem<int>> data{{0, 4}, {1, 3}, {2, 2}, {3, 1}, {4, 0}};
  PriorityQueue<int> queue(5);
  for (auto v : data) {
    queue.Insert(v.item, v.weight);
  }
  ASSERT_EQUAL(queue.GetItem(4).item, 4);
  ASSERT_EQUAL(queue.GetItem(4).weight, 0);
  ASSERT_EQUAL(queue.GetItem(2).item, 2);
  ASSERT_EQUAL(queue.GetItem(2).weight, 2);
}

void TestInsertQueue() {
  vector<QueueItem<int>> data{{0, 4}, {1, 3}, {2, 2}, {3, 1}, {4, 0}};
  PriorityQueue<int> queue(5);
  for (auto v : data) {
    queue.Insert(v.item, v.weight);
  }
  ASSERT_EQUAL(queue.Size(), 5UL);
  auto max = queue.PopMax();
  ASSERT_EQUAL(queue.Size(), 4UL);
  ASSERT_EQUAL(max.item, 0);
  ASSERT_EQUAL(max.weight, 4);
}

void TestUpdatePriorityQueue() {
  vector<QueueItem<int>> data{{0, 5}, {1, 4}, {2, 3}, {3, 2}, {4, 1}};
  PriorityQueue<int> queue(5);
  for (auto v : data) {
    queue.Insert(v.item, v.weight);
  }
  queue.UpdatePriority(1, 10);
  auto max = queue.PopMax();
  ASSERT_EQUAL(max.item, 1);
  ASSERT_EQUAL(max.weight, 10);
  max = queue.PopMax();
  ASSERT_EQUAL(max.item, 0);
  ASSERT_EQUAL(max.weight, 5);
  ASSERT_EQUAL(queue.Size(), 3UL); // (2,3),(3,2),(4,1)
  queue.Insert(199, 0);
  ASSERT_EQUAL(queue.Size(), 4UL); // (2,3),(3,2),(4,1),(199,0)
  ASSERT_EQUAL(queue.PeekMin().item, 199);
  ASSERT_EQUAL(queue.PeekMin().weight, 0);
  queue.PopMin();
  ASSERT_EQUAL(queue.Size(), 3UL); // (2,3),(3,2),(4,1)
  queue.Insert(199, 2.5);
  ASSERT_EQUAL(queue.Size(), 4UL); // (2,3), (199, 2.5), (3,2), (4,1)
  ASSERT_EQUAL(queue.PeekMax().item, 2);
  ASSERT_EQUAL(queue.PeekMax().weight, 3);
  queue.Insert(199, 5.5);
  ASSERT_EQUAL(queue.Size(), 5UL); // (199, 5.5), (2,3), (199, 2.5), (3,2), (4,1)
  ASSERT_EQUAL(queue.PeekMax().item, 199);
  ASSERT_EQUAL(queue.PeekMax().weight, 5.5);
  ASSERT_EQUAL(queue.PeekMin().item, 4);
  ASSERT_EQUAL(queue.PeekMin().weight, 1);
}
