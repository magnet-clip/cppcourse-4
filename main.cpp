#include "profile.h"
#include "test_runner.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

using namespace std;

template <typename T>
void PrintCoeff(std::ostream &out, int i, const T &coef, bool printed) {
  bool coeffPrinted = false;
  if (coef == 1 && i > 0) {
    out << (printed ? "+" : "");
  } else if (coef == -1 && i > 0) {
    out << "-";
  } else if (coef >= 0 && printed) {
    out << "+" << coef;
    coeffPrinted = true;
  } else {
    out << coef;
    coeffPrinted = true;
  }
  if (i > 0) {
    out << (coeffPrinted ? "*" : "") << "x";
  }
  if (i > 1) {
    out << "^" << i;
  }
}
template <typename T> class Writer;

template <typename T> class Polynomial {
  friend class Writer<T>;

private:
  vector<T> coeffs_ = {0};

  void Shrink() {
    while (coeffs_.size() > 1 && coeffs_.back() == 0) {
      coeffs_.pop_back();
    }
  }

public:
  Polynomial() = default;
  Polynomial(vector<T> coeffs) : coeffs_(std::move(coeffs)) { Shrink(); }

  template <typename Iterator>
  Polynomial(Iterator first, Iterator last) : coeffs_(first, last) {
    Shrink();
  }

  bool operator==(const Polynomial &other) const {
    return coeffs_ == other.coeffs_;
  }

  bool operator!=(const Polynomial &other) const { return !operator==(other); }

  int Degree() const { return coeffs_.size() - 1; }

  Polynomial &operator+=(const Polynomial &r) {
    if (r.coeffs_.size() > coeffs_.size()) {
      coeffs_.resize(r.coeffs_.size());
    }
    for (size_t i = 0; i != r.coeffs_.size(); ++i) {
      coeffs_[i] += r.coeffs_[i];
    }
    Shrink();
    return *this;
  }

  Polynomial &operator-=(const Polynomial &r) {
    if (r.coeffs_.size() > coeffs_.size()) {
      coeffs_.resize(r.coeffs_.size());
    }
    for (size_t i = 0; i != r.coeffs_.size(); ++i) {
      coeffs_[i] -= r.coeffs_[i];
    }
    Shrink();
    return *this;
  }

  T operator[](size_t degree) const {
    return degree < coeffs_.size() ? coeffs_[degree] : 0;
  }

  Writer<T> operator[](size_t degree) { return Writer<T>(*this, degree); }

  T operator()(const T &x) const {
    T res = 0;
    for (auto it = coeffs_.rbegin(); it != coeffs_.rend(); ++it) {
      res *= x;
      res += *it;
    }
    return res;
  }

  using const_iterator = typename std::vector<T>::const_iterator;

  const_iterator begin() const { return coeffs_.cbegin(); }

  const_iterator end() const { return coeffs_.cend(); }
};

template <typename T> class Writer {

public:
  Writer(Polynomial<T> &poly, size_t pos) : _pos(pos), _poly(poly) {
    if (_poly.coeffs_.size() <= _pos) { // should we not resize it immediately?
      _value = 0;
    } else {
      _value = _poly.coeffs_[_pos];
    }
  }

  operator T() const { return std::as_const(_poly)[_pos]; }

  T &operator=(T value) {
    if (value == 0) {
      if (_poly.coeffs_.size() > _pos) {
        _poly.coeffs_[_pos] = 0;
        _poly.Shrink();
      }
      _value = 0;
    } else {
      if (_poly.coeffs_.size() <= _pos) {
        _poly.coeffs_.resize(_pos + 1);
      }
      _value = _poly.coeffs_[_pos] = value;
    }
    return _value;
  }

  bool operator==(T value) const { return Get() == value; }
  bool operator!=(T value) const { return Get() != value; }
  const T Get() const {
    return _pos >= _poly.coeffs_.size() ? 0 : _poly.coeffs_[_pos];
  }

private:
  size_t _pos;
  Polynomial<T> &_poly;
  T _value;
};

template <typename T> ostream &operator<<(ostream &s, Writer<T> w) {
  return s << w.Get();
}
template <typename T>
std::ostream &operator<<(std::ostream &out, const Polynomial<T> &p) {
  bool printed = false;
  for (int i = p.Degree(); i >= 0; --i) {
    if (p[i] != 0) {
      PrintCoeff(out, i, p[i], printed);
      printed = true;
    }
  }
  return out;
}

template <typename T>
Polynomial<T> operator+(Polynomial<T> lhs, const Polynomial<T> &rhs) {
  lhs += rhs;
  return lhs;
}

template <typename T>
Polynomial<T> operator-(Polynomial<T> lhs, const Polynomial<T> &rhs) {
  lhs -= rhs;
  return lhs;
}

void TestCreation() {
  {
    Polynomial<int> default_constructed;
    ASSERT_EQUAL(default_constructed.Degree(), 0);
    ASSERT_EQUAL(default_constructed[0], 0);
  }
  {
    Polynomial<double> from_vector({1.0, 2.0, 3.0, 4.0});
    ASSERT_EQUAL(from_vector.Degree(), 3);
    ASSERT_EQUAL(from_vector[0], 1.0);
    ASSERT_EQUAL(from_vector[1], 2.0);
    ASSERT_EQUAL(from_vector[2], 3.0);
    ASSERT_EQUAL(from_vector[3], 4.0);
  }
  {
    const vector<int> coeffs = {4, 9, 7, 8, 12};
    Polynomial<int> from_iterators(begin(coeffs), end(coeffs));
    ASSERT_EQUAL(from_iterators.Degree(), coeffs.size() - 1);
    ASSERT(std::equal(cbegin(from_iterators), cend(from_iterators),
                      begin(coeffs)));
  }
}

void TestEqualComparability() {
  Polynomial<int> p1({9, 3, 2, 8});
  Polynomial<int> p2({9, 3, 2, 8});
  Polynomial<int> p3({1, 2, 4, 8});

  ASSERT_EQUAL(p1, p2);
  ASSERT(p1 != p3);
}

void TestAddition() {
  Polynomial<int> p1({9, 3, 2, 8});
  Polynomial<int> p2({1, 2, 4});

  p1 += p2;
  ASSERT_EQUAL(p1, Polynomial<int>({10, 5, 6, 8}));

  auto p3 = p1 + p2;
  ASSERT_EQUAL(p3, Polynomial<int>({11, 7, 10, 8}));

  p3 += Polynomial<int>({-11, 1, -10, -8});
  ASSERT_EQUAL(p3.Degree(), 1);
  const Polynomial<int> expected{{0, 8}};
  ASSERT_EQUAL(p3, expected);
}

void TestSubtraction() {
  Polynomial<int> p1({9, 3, 2, 8});
  Polynomial<int> p2({1, 2, 4});

  p1 -= p2;
  ASSERT_EQUAL(p1, Polynomial<int>({8, 1, -2, 8}));

  auto p3 = p1 - p2;
  ASSERT_EQUAL(p3, Polynomial<int>({7, -1, -6, 8}));

  p3 -= Polynomial<int>({7, -5, -6, 8});
  ASSERT_EQUAL(p3.Degree(), 1);
  const Polynomial<int> expected{{0, 4}};
  ASSERT_EQUAL(p3, expected);
}

void TestEvaluation() {
  const Polynomial<int> default_constructed;
  ASSERT_EQUAL(default_constructed(0), 0);
  ASSERT_EQUAL(default_constructed(1), 0);
  ASSERT_EQUAL(default_constructed(-1), 0);
  ASSERT_EQUAL(default_constructed(198632), 0);

  const Polynomial<int64_t> cubic({1, 1, 1, 1});
  ASSERT_EQUAL(cubic(0), 1);
  ASSERT_EQUAL(cubic(1), 4);
  ASSERT_EQUAL(cubic(2), 15);
  ASSERT_EQUAL(cubic(21), 9724);
}

void TestConstAccess() {
  const Polynomial<int> poly(std::initializer_list<int>{1, 2, 3, 4, 5});

  ASSERT_EQUAL(poly[0], 1);
  ASSERT_EQUAL(poly[1], 2);
  ASSERT_EQUAL(poly[2], 3);
  ASSERT_EQUAL(poly[3], 4);
  ASSERT_EQUAL(poly[4], 5);
  ASSERT_EQUAL(poly[5], 0);
  ASSERT_EQUAL(poly[6], 0);
  ASSERT_EQUAL(poly[608], 0);
}

void TestNonconstAccess() {
  Polynomial<int> poly;

  poly[0] = 1;
  poly[3] = 12;
  poly[5] = 4;
  ASSERT_EQUAL(poly.Degree(), 5);

  ASSERT_EQUAL(poly[0], 1);
  ASSERT_EQUAL(poly[1], 0);
  ASSERT_EQUAL(poly[2], 0);
  ASSERT_EQUAL(poly[3], 12);
  ASSERT_EQUAL(poly[4], 0);
  ASSERT_EQUAL(poly[5], 4);
  ASSERT_EQUAL(poly[6], 0);
  ASSERT_EQUAL(poly[608], 0);

  ASSERT_EQUAL(poly.Degree(), 5);

  poly[608] = 0;
  ASSERT_EQUAL(poly.Degree(), 5);

  {
    LOG_DURATION("Iteration over const");
    for (size_t i = 10; i < 50000; ++i) {
      ASSERT_EQUAL(std::as_const(poly)[i], 0);
      ASSERT_EQUAL(poly.Degree(), 5);
    }
  }
  {
    LOG_DURATION("Iteration over non-const");
    for (size_t i = 10; i < 50000; ++i) {
      ASSERT_EQUAL(poly[i], 0);
      ASSERT_EQUAL(poly.Degree(), 5);
    }
  }
}

void TestNoChange() {
  Polynomial<int> p;
  p[2] = 1;
  ASSERT_EQUAL(p.Degree(), 2);
  int x = p[5];
  ASSERT_EQUAL(x, 0);
  ASSERT_EQUAL(p.Degree(), 2);
}

void TestInitialExpand() {
  Polynomial<int> p;
  ASSERT_EQUAL(p.Degree(), 0);
  auto z = p[1];
  ASSERT_EQUAL(p.Degree(), 0);
  z = 2;
  ASSERT_EQUAL(p.Degree(), 1);
  int q = p[2];
  ASSERT_EQUAL(q, 0);
  ASSERT_EQUAL(p.Degree(), 1);
  q = 3;
  ASSERT_EQUAL(p.Degree(), 1);
  ASSERT_EQUAL(p[2], 0);
}

void TestShrink() {
  Polynomial<int> p;
  p[2] = 1;
  ASSERT_EQUAL(p.Degree(), 2);
  p[3] = 2;
  ASSERT_EQUAL(p.Degree(), 3);
  p[5] = 7;
  ASSERT_EQUAL(p.Degree(), 5);
  auto x = p[5];
  ASSERT_EQUAL(x, 7);
  x = 0;
  ASSERT_EQUAL(p.Degree(), 3);
  p[3] = 0;
  ASSERT_EQUAL(p.Degree(), 2);
  p[1] = 100;
  ASSERT_EQUAL(p.Degree(), 2);
  p[2] = 0;
  ASSERT_EQUAL(p.Degree(), 1);
  p[1] = 0;
  ASSERT_EQUAL(p.Degree(), 0);
  p[3] = 2;
  ASSERT_EQUAL(p.Degree(), 3);
  auto q = p[4];
  ASSERT_EQUAL(p.Degree(), 3);
  q = 1000;
  ASSERT_EQUAL(p.Degree(), 4);
  ASSERT_EQUAL(q, 1000);
  p[4] = 0;
  ASSERT_EQUAL(p.Degree(), 3);
  ASSERT_EQUAL(q, 0);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestCreation);
  RUN_TEST(tr, TestEqualComparability);
  RUN_TEST(tr, TestAddition);
  RUN_TEST(tr, TestSubtraction);
  RUN_TEST(tr, TestEvaluation);
  RUN_TEST(tr, TestConstAccess);
  RUN_TEST(tr, TestNonconstAccess);
  RUN_TEST(tr, TestNoChange);
  RUN_TEST(tr, TestInitialExpand);
  RUN_TEST(tr, TestShrink);
  return 0;
}
