#include "main.h"
#include "command.h"
#include "command.test.h"
#include "database.test.h"
#include "geomath.test.h"
#include "query.test.h"
#include "test_runner.h"

using namespace std;

void RunAllTests() {
  TestRunner tr;
  RUN_TEST(tr, TestDistance);
  RUN_TEST(tr, TestStopCommandParsing);
  RUN_TEST(tr, TestBusCommandParsing);
  RUN_TEST(tr, TestStopCommandParsingExtended);
  RUN_TEST(tr, TestBusQueryParsing);
  RUN_TEST(tr, TestSample3);
}

void InAndOut(istream &is = cin, ostream &os = cout,
              const Serializer &serializer = JsonSerializer());

int main() {
  RunAllTests();
  InAndOut(cin, cout, StringSerializer());
  return 0;
}