#include "hash_set.h"
#include "test_runner.h"

#include <future>
#include <random>

struct IntHasher {
  size_t operator()(int value) const {
    // Это реальная хеш-функция из libc++, libstdc++.
    // Чтобы она работала хорошо, std::unordered_map
    // использует простые числа для числа бакетов
    return value;
  }
};

struct TestValue {
  int value;

  bool operator==(TestValue other) const {
    return value / 2 == other.value / 2;
  }
};

struct TestValueHasher {
  size_t operator()(TestValue value) const { return value.value / 2; }
};

void TestSmoke() {
  HashSet<int, IntHasher> hash_set(2);
  hash_set.Add(3);
  hash_set.Add(4);

  ASSERT(hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(!hash_set.Has(5));

  hash_set.Erase(3);

  ASSERT(!hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(!hash_set.Has(5));

  hash_set.Add(3);
  hash_set.Add(5);

  ASSERT(hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(hash_set.Has(5));
}

void TestEmpty() {
  HashSet<int, IntHasher> hash_set(10);
  for (int value = 0; value < 10000; ++value) {
    ASSERT(!hash_set.Has(value));
  }
}

void TestIdempotency() {
  HashSet<int, IntHasher> hash_set(10);
  hash_set.Add(5);
  ASSERT(hash_set.Has(5));
  hash_set.Add(5);
  ASSERT(hash_set.Has(5));
  hash_set.Erase(5);
  ASSERT(!hash_set.Has(5));
  hash_set.Erase(5);
  ASSERT(!hash_set.Has(5));
}

void TestEquivalence() {
  HashSet<TestValue, TestValueHasher> hash_set(10);
  hash_set.Add(TestValue{2});
  hash_set.Add(TestValue{3});

  ASSERT(hash_set.Has(TestValue{2}));
  ASSERT(hash_set.Has(TestValue{3}));

  const auto &bucket = hash_set.GetBucket(TestValue{2});
  const auto &three_bucket = hash_set.GetBucket(TestValue{3});
  ASSERT_EQUAL(&bucket, &three_bucket);

  ASSERT_EQUAL(1, distance(begin(bucket), end(bucket)));
  ASSERT_EQUAL(2, bucket.front().value);
}

void TestRandomizedMultithreaded() {
  HashSet<int, IntHasher> hash_set(1000);
  const int num_threads = 100;
  const int num_actions = 100'000;
  const int seed = 43;

  mutex random_mutex;
  default_random_engine rd(seed);
  uniform_int_distribution<int> next_int;
  uniform_int_distribution<int> next_action(0, 2);

  vector<future<void>> threads;
  for (int i = 0; i < num_threads; i++) {
    threads.push_back(async([&hash_set, num_actions, &random_mutex, &rd,
                             &next_int, &next_action]() {
      for (int i = 0; i < num_actions; i++) {
        int value;
        int action;
        {
          lock_guard guard(random_mutex);
          value = next_int(rd);
          action = next_action(rd);
        }

        if (action == 0) {
          // Insert
          hash_set.Add(value);
        } else if (action == 1) {
          // Remove
          hash_set.Erase(value);
        } else {
          // Check if exists
          hash_set.Has(value);
        }
      }
    }));
  }
}

int main() {
  // TestRunner tr;
  // RUN_TEST(tr, TestSmoke);
  // RUN_TEST(tr, TestEmpty);
  // RUN_TEST(tr, TestIdempotency);
  // RUN_TEST(tr, TestEquivalence);
  // RUN_TEST(tr, TestRandomizedMultithreaded);
  return 0;
}