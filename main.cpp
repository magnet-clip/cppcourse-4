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
}

vector<string> Execute(const vector<CommandPtr> &commands,
                       const vector<QueryPtr> &queries) {
  Database db{commands};
  vector<string> res;
  for (const auto &q : queries) {
    const BusQuery &bus_query = static_cast<const BusQuery &>(*q);
    auto response = db.ExecuteBusQuery(bus_query);
    res.push_back(response->ToString());
  }
  return res;
}

int main() {
  // RunAllTests();
  auto commands = ReadCommands();
  auto queries = ReadQueries();
  auto res = Execute(commands, queries);
  for (const auto &out : res) {
    cout << out << endl;
  }
  return 0;
}