#include "database.test.h"
#include "geomath.test.h"
#include "io.h"
#include "io.test.h"
#include "json.test.h"
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
  RUN_TEST(tr, TestJsonBusParsing);
  RUN_TEST(tr, TestJsonSample);
  RUN_TEST(tr, TestJsonSerialization);
}

int main() {
  // RunAllTests();
  JsonIo io;
  InAndOut(cin, cout, io);
  return 0;
}