#include "Common.h"
#include "test_runner.h"

#include <functional>
#include <sstream>
#include <string>

using namespace std;

class ValueExpression : public Expression {
 public:
  ValueExpression(int value);
  virtual int Evaluate() const override;
  virtual string ToString() const override;

 private:
  int _value;
};

ValueExpression::ValueExpression(int value) : _value(value) {}
int ValueExpression::Evaluate() const { return _value; }
string ValueExpression::ToString() const {
  return "(" + to_string(_value) + ")";
}

template <typename BinaryOperation, char op>
class BinaryExpression : public Expression {
 public:
  BinaryExpression(ExpressionPtr expr1, ExpressionPtr expr2);
  virtual int Evaluate() const override;
  virtual string ToString() const override;

 private:
  ExpressionPtr _expr1, _expr2;
};

template <typename BinaryOperation, char op>
BinaryExpression<BinaryOperation, op>::BinaryExpression(ExpressionPtr expr1,
                                                        ExpressionPtr expr2)
    : _expr1(move(expr1)), _expr2(move(expr2)) {}

template <typename BinaryOperation, char op>
string BinaryExpression<BinaryOperation, op>::ToString() const {
  return "(" + _expr1->ToString() + op + _expr2->ToString() + ")";
}

template <typename BinaryOperation, char op>
int BinaryExpression<BinaryOperation, op>::Evaluate() const {
  return BinaryOperation{}(_expr1->Evaluate(), _expr2->Evaluate());
}

class SumExpression : public BinaryExpression<plus<int>, '+'> {
 public:
  using BinaryExpression::BinaryExpression;
};

class ProductExpression : public BinaryExpression<multiplies<int>, '*'> {
 public:
  using BinaryExpression::BinaryExpression;
};

ExpressionPtr Value(int value) {
  return ExpressionPtr(new ValueExpression(value));
}
ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
  return ExpressionPtr(new SumExpression(move(left), move(right)));
}
ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
  return ExpressionPtr(new ProductExpression(move(left), move(right)));
}

string Print(const Expression *e) {
  if (!e) {
    return "Null expression provided";
  }
  stringstream output;
  output << e->ToString() << " = " << e->Evaluate();
  return output.str();
}

void Test() {
  ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
  ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

  ExpressionPtr e2 = Sum(move(e1), Value(5));
  ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

  ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, Test);
  return 0;
}