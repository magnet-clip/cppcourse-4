#include "json.test.h"

#include "json.h"
#include "test_runner.h"

#include <sstream>

using namespace std;
using namespace Json;

void TestJsonSerialization() {
  {
    stringstream s;
    s << "{\"a\" : 1}";
    auto doc = Load(s);
    ASSERT_EQUAL(doc.GetRoot().ToString(), "{\"a\":1}");
  }
  {
    stringstream s;
    s << "{\"a\" : -11}";
    auto doc = Load(s);
    ASSERT_EQUAL(doc.GetRoot().ToString(), "{\"a\":-11}");
  }
}
