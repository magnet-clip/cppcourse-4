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
    s << "{\"a\":1.01}";
    auto doc = Load(s);
    ASSERT_EQUAL(doc.GetRoot().ToString(), "{\"a\":1.01}");
  }
  {
    stringstream s;
    s << "{\"a\" : -11}";
    auto doc = Load(s);
    ASSERT_EQUAL(doc.GetRoot().ToString(), "{\"a\":-11}");
  }
  {
    stringstream s;
    s << "{\"a\":-11.1}";
    auto doc = Load(s);
    ASSERT_EQUAL(doc.GetRoot().ToString(), "{\"a\":-11.1}");
  }
  {
    auto str =
        "{\"a\":1,\"b\":-1,\"c\":1.01,\"d\":-1.01,\"e\":true,\"f\":false,\"g\":"
        "\"a\",\"h\":[1,2.01],\"i\":{\"a\":[true,false]}}"s;
    stringstream s;
    s << str;
    auto doc = Load(s);
    ASSERT_EQUAL(doc.GetRoot().ToString(), str);
  }
}
