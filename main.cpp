#include "command.h"
#include "command.test.h"
#include "database.h"
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
  RUN_TEST(tr, TestSample2);
}

int main() {
  RunAllTests();
  auto commands = ReadCommands();
  auto queries = ReadQueries();

  Database db;
  db.ExecuteCommands(commands);
  auto res = db.ExecuteQueries(queries);
  for (const auto &out : res) {
    cout << out->ToString() << endl;
  }
  return 0;
}