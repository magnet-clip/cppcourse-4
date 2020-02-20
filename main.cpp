
#include "test.h"
#include "test_runner.h"

using namespace std;

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestRangeBoundaries);
  RUN_TEST(tr, TestSameUser);
  RUN_TEST(tr, TestRangeBoundaries);

  RUN_TEST(tr, TestDoubleInsert);
  RUN_TEST(tr, TestGetByIdAndErase);
  RUN_TEST(tr, TestTimeIntervals);
  RUN_TEST(tr, TestDoubleInsertSecondaryKey);
  RUN_TEST(tr, TestDoubleSearchByName);
  return 0;
}
