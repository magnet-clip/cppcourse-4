#include "test_runner.h"

#include <functional>
#include <string>
using namespace std;

template <typename T>
class LazyValue final {
 public:
  explicit LazyValue(function<T()> init) : _init(init) {}
  ~LazyValue() {
    if (_initialized) {
      delete _value;
    }
  }

  bool HasValue() const { return _initialized; }
  const T& Get() const {
    if (!_initialized) {
      _value = new T(_init());
      _initialized = true;
    }
    return *_value;
  }

 private:
  function<T()> _init;
  mutable T* _value;
  mutable bool _initialized = false;
};

void UseExample() {
  const string big_string = "Giant amounts of memory";

  LazyValue<string> lazy_string([&big_string] { return big_string; });

  ASSERT(!lazy_string.HasValue());
  ASSERT_EQUAL(lazy_string.Get(), big_string);
  ASSERT_EQUAL(lazy_string.Get(), big_string);
}

void TestInitializerIsntCalled() {
  bool called = false;

  {
    LazyValue<int> lazy_int([&called] {
      called = true;
      return 0;
    });
  }
  ASSERT(!called);
}

class Xxx {
 public:
  Xxx() = delete;
  explicit Xxx(int i) : _i(i) {}

 private:
  int _i;
};

void TestNoDefaultConstructor() {
  bool called;
  LazyValue<Xxx> lazy_xxx([&called] {
    called = true;
    return Xxx(0);
  });
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, UseExample);
  RUN_TEST(tr, TestInitializerIsntCalled);
  RUN_TEST(tr, TestNoDefaultConstructor);
  return 0;
}
