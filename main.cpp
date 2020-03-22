#include "command.h"
#include "command.test.h"
#include "database.test.h"
#include "geomath.test.h"
#include "query.h"
#include "query.test.h"
#include "test_runner.h"

#include <iostream>
using namespace std;

void RunAllTests() {
  TestRunner tr;
  RUN_TEST(tr, TestDistance);
  RUN_TEST(tr, TestStopCommandParsing);
  RUN_TEST(tr, TestBusCommandParsing);
  RUN_TEST(tr, TestBusQueryParsing);
  RUN_TEST(tr, TestSample);
}

int main() {
  RunAllTests();
  auto commands = ReadCommands();
  auto queries = ReadQueries();
  return 0;
}