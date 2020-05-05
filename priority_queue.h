#pragma once

#include <algorithm>
#include <optional>
#include <unordered_map>
#include <vector>

template <typename Data>
struct QueueItem {
  Data item;
  double weight;
};

template <typename Data>
class PriorityQueue {
public:
  PriorityQueue(size_t max_size) : _heap(max_size),
                                   _size(0),
                                   _max_size(max_size) {
  }

  void Insert(const Data &data, double weight) {
    if (_size == _max_size) {
      throw std::domain_error("Too many elements");
    }
    _size += 1;
    _heap[_size] = {data, weight};
    _pos[data] = _size;
    SiftUp(_size);
  }

  QueueItem<Data> PopMax() {
    QueueItem<Data> result = _heap[0];
    _heap[0] = _heap[_size--];
    SiftDown(0);
    return result;
  }

  QueueItem<Data> PopMin() {
    return _heap[_size--];
  }

  const QueueItem<Data> &PeekMax() {
    return _heap[0];
  }

  const QueueItem<Data> &PeekMin() {
    return _heap[_size];
  }

  const QueueItem<Data> &GetItem(Data item) {
    return _heap[_pos[item]];
  }

  size_t Size() { return _size; }

  void UpdatePriority(Data data, double weight) {
    _heap[_pos[data]].weight = weight;
    Swap(_pos[data], 0);
    SiftDown(0);
  }

private:
  size_t Parent(size_t i) { return (i - 1) / 2; }
  size_t LeftChild(size_t i) { return 2 * i + 1; }
  size_t RightChild(size_t i) { return 2 * i + 2; }
  void Swap(size_t i, size_t j) {
    std::swap(_pos[_heap[i].item], _pos[_heap[j].item]);
    std::swap(_heap[i], _heap[j]);
  }

  void SiftUp(size_t i) {
    while (i > 0 && _heap[Parent(i)].weight < _heap[i].weight) {
      auto p = Parent(i);
      Swap(i, p);
      i = p;
    }
  }

  void SiftDown(size_t i) {
    auto max_index = i;
    auto l = LeftChild(i);
    if (l < _size && _heap[l].weight > _heap[max_index].weight) {
      max_index = l;
    }
    auto r = RightChild(i);
    if (r < _size && _heap[r].weight > _heap[max_index].weight) {
      max_index = r;
    }
    if (i != max_index) {
      Swap(i, max_index);
      SiftDown(max_index);
    }
  }

  std::vector<QueueItem<Data>> _heap;    // Position -> VertexId, weight
  std::unordered_map<Data, size_t> _pos; // VertexId -> Position
  size_t _size;
  size_t _max_size;
};
