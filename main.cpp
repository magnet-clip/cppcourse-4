#include <fstream>

#include "database.test.h"
#include "geomath.test.h"
#include "io.h"
#include "io.test.h"
#include "json.test.h"
#include "parser.test.h"
#include "priority_queue.test.h"
#include "test_runner.h"

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
  RUN_TEST(tr, TestJsonSerialization);
  RUN_TEST(tr, TestVV);
  RUN_TEST(tr, TestCreateQueue);
  RUN_TEST(tr, TestGetItemsQueue);
  RUN_TEST(tr, TestInsertQueue);
  RUN_TEST(tr, TestUpdatePriorityQueue);
  RUN_TEST(tr, TestRemoveAllElements);
  RUN_TEST(tr, TestSameWeight);
}

void TestFiles(const string &name, const string &input, const string &output) {
  ifstream f_input;
  ifstream f_output;

  f_input.open(input);
  f_output.open(output);
  if (!f_input.is_open() || !f_output.is_open()) {
    throw domain_error("Failed to open test files");
  }
  RunIntegrationTest(name, f_input, f_output);
}

void TestPartD() { TestFiles("d", "test-d1-i.json", "test-d1-o.json"); }
void TestPartE1() { TestFiles("e1", "test-e1-input.json", "test-e1-output.json"); }
void TestPartE2() { TestFiles("e2", "test-e2-input.json", "test-e2-output.json"); }
void TestPartE3() { TestFiles("e3", "test-e3-input.json", "test-e3-output.json"); }
void TestPartF() { TestFiles("all", "transport-input4.json", "transport-output4.json"); }

void RunIntegrationTests() {
  TestRunner tr;
  // RUN_TEST(tr, TestPartD);
  RUN_TEST(tr, TestPartE1);
  // RUN_TEST(tr, TestPartE2);
  // RUN_TEST(tr, TestPartE3);
  // RUN_TEST(tr, TestPartF);
}

int main() {
  RunUnitTests();
  RunIntegrationTests();
  // JsonIo io;
  // InAndOut(cin, cout, io);
  return 0;
}