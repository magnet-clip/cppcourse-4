#include "command.h"
#include "command.test.h"
#include "query.h"
#include "query.test.h"
#include "test_runner.h"

#include <iostream>
using namespace std;

void RunAllTests() {
  TestRunner tr;
  RUN_TEST(tr, TestStopCommandParsing);
  RUN_TEST(tr, TestBusCommandParsing);
  RUN_TEST(tr, TestBusQueryParsing);
}

int main() {
  RunAllTests();
  auto commands = ReadCommands();
  auto queries = ReadQueries();
  return 0;
}