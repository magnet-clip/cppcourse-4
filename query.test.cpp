#include "query.test.h"
#include "io.h"
#include "query.h"
#include "test_runner.h"

using namespace std;

void TestBusQueryParsing() {
  stringstream cmd_stream;
  cmd_stream << "3" << endl
             << "Bus 256" << endl
             << "Bus 750" << endl
             << "Bus 751" << endl;

  auto res = ReadQueries(cmd_stream, StringParser());
  ASSERT_EQUAL(res.size(), 3U);

  {
    ASSERT_EQUAL(res[0]->Kind(), Queries::BusQuery);
    auto bus = static_cast<BusQuery &>(*res[0]);
    ASSERT_EQUAL(bus.GetName(), "256");
  }
  {
    ASSERT_EQUAL(res[1]->Kind(), Queries::BusQuery);
    auto bus = static_cast<BusQuery &>(*res[1]);
    ASSERT_EQUAL(bus.GetName(), "750");
  }
  {
    ASSERT_EQUAL(res[2]->Kind(), Queries::BusQuery);
    auto bus = static_cast<BusQuery &>(*res[2]);
    ASSERT_EQUAL(bus.GetName(), "751");
  }
}
