#include "command.h"
#include "command.test.h"
#include "test_runner.h"

#include <iostream>
using namespace std;

void RunAllTests() {
  TestRunner tr;
  RUN_TEST(tr, TestStopCommandParsing);
  RUN_TEST(tr, TestBusCommandParsing);
}

int main() {
  RunAllTests();
  auto commands = ReadCommands();
  cout << commands[0]->ToString() << endl;
  return 0;
}