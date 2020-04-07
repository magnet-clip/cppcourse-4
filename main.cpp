#include "database.test.h"
#include "geomath.test.h"
#include "io.h"
#include "io.test.h"
#include "json.test.h"
#include "parser.test.h"
#include "test_runner.h"

#include <fstream>

using namespace std;

void RunUnitTests() {
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

void TestFiles(const string &input, const string &output) {
  ifstream f_input(input);
  ifstream f_output(output);
  if (!f_input.is_open()) {
    throw domain_error("closed");
  }
  RunIntegrationTest(f_input, f_output);
}

void TestPartD() {
  TestFiles("../test-d1-input.json", "../test-d1-output.json");
}
void TestPartE1() {
  TestFiles("../test-e1-input.json", "../test-e1-output.json");
}

void RunIntegrationTests() {
  TestRunner tr;
  RUN_TEST(tr, TestPartD);
  RUN_TEST(tr, TestPartE1);
}

int main() {
  RunUnitTests();
  RunIntegrationTests();
  JsonIo io;
  InAndOut(cin, cout, io);
  return 0;
}