#include "database.test.h"
#include "geomath.test.h"
#include "io.h"
#include "parser.test.h"
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
  RUN_TEST(tr, TestJsonStopParsing);
}

int main() {
  RunAllTests();
  StringIo().InAndOut(cin, cout);
  return 0;
}