#include "command.test.h"
#include "command.h"
#include "geomath.h"
#include "test_runner.h"

#include <sstream>

using namespace std;

void TestStopCommandParsing() {
  {
    stringstream cmd_stream;
    cmd_stream << "1" << endl
               << "Stop Tolstopaltsevo: 55.611087, 37.20829" << endl;
    auto res = ReadCommands(cmd_stream);
    auto cmd_ptr = res[0];
    ASSERT_EQUAL(cmd_ptr->Kind(), Commands::StopCommand);
    auto &cmd_stop = static_cast<StopCommand &>(*cmd_ptr);
    GeoPoint expected{Latitude(55.611087), Longitude(37.20829)};
    ASSERT_EQUAL(cmd_stop.GetName(), "Tolstopaltsevo");
    ASSERT_EQUAL(cmd_stop.GetLocation(), expected);
  }
  {
    stringstream cmd_stream;
    cmd_stream << "1" << endl
               << "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517" << endl;
    auto res = ReadCommands(cmd_stream);
    auto cmd_ptr = res[0];
    ASSERT_EQUAL(cmd_ptr->Kind(), Commands::StopCommand);
    auto &cmd_stop = static_cast<StopCommand &>(*cmd_ptr);
    ASSERT_EQUAL(cmd_stop.GetName(), "Biryulyovo Zapadnoye");
    GeoPoint expected{Latitude(55.574371), Longitude(37.6517)};
    ASSERT_EQUAL(cmd_stop.GetLocation(), expected);
  }
}