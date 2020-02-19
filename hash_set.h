#pragma once

#include <algorithm>
#include <forward_list>
#include <iterator>
#include <shared_mutex>
#include <vector>
using namespace std;

template <typename Type, typename Hasher> class HashSet {
public:
  using BucketList = forward_list<Type>;

private:
  struct Buckets {
  private:
    vector<BucketList> _buckets;
    const size_t _buckets_count;

    size_t GetPosition(const size_t hash) const {
      return hash % _buckets_count;
    }

  public:
    Buckets(size_t num_buckets)
        : _buckets(num_buckets), _buckets_count(num_buckets) {}

    void Add(const size_t hash, const Type &value) {
      auto idx = GetPosition(hash);
      auto &bucket = _buckets[idx];
      auto it = find(bucket.begin(), bucket.end(), value);
      if (it == bucket.end()) {
        bucket.push_front(value);
      }
    }

    void Erase(const size_t hash, const Type &value) {
      auto idx = GetPosition(hash);
      auto &bucket = _buckets[idx];
      bucket.remove(value);
    };

    bool Has(const size_t hash, const Type &value) const {
      auto idx = GetPosition(hash);
      auto &bucket = _buckets[idx];
      auto it = find(bucket.begin(), bucket.end(), value);
      return it != bucket.end();
    }

    const BucketList &GetBucket(const size_t hash) const {
      auto idx = GetPosition(hash);
      auto &bucket = _buckets[idx];
      return bucket;
    }
  };

  const Hasher &_hasher;
  Buckets _buckets;
  mutable shared_mutex _m;

public:
  explicit HashSet(size_t num_buckets, const Hasher &hasher = {})
      : _hasher(hasher), _buckets(num_buckets) {}

  void Add(const Type &value) {
    unique_lock lock(_m);
    _buckets.Add(_hasher(value), value);
  }

  void Erase(const Type &value) {
    unique_lock lock(_m);
    _buckets.Erase(_hasher(value), value);
  }

  bool Has(const Type &value) const {
    shared_lock lock(_m);
    return _buckets.Has(_hasher(value), value);
  }

  const BucketList &GetBucket(const Type &value) const {
    shared_lock lock(_m);
    return _buckets.GetBucket(_hasher(value));
  }
};