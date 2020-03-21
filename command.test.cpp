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

void TestBusCommandParsing() {
  stringstream cmd_stream;
  cmd_stream
      << "2" << endl
      << "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo "
         "Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"
      << endl
      << "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka" << endl;
  auto res = ReadCommands(cmd_stream);
  ASSERT_EQUAL(res[0]->Kind(), Commands::BusCommand);
  auto bus256 = static_cast<BusCommand &>(*res[0]);
  ASSERT_EQUAL(bus256.GetNumber(), 256);
  ASSERT(bus256.IsCircular());
  ASSERT_EQUAL(bus256.GetStops().size(), 6U);
  ASSERT_EQUAL(bus256.GetStops()[0], "Biryulyovo Zapadnoye");
  ASSERT_EQUAL(bus256.GetStops()[1], "Biryusinka");
  ASSERT_EQUAL(bus256.GetStops()[2], "Universam");
  ASSERT_EQUAL(bus256.GetStops()[3], "Biryulyovo Tovarnaya");
  ASSERT_EQUAL(bus256.GetStops()[4], "Biryulyovo Passazhirskaya");
  ASSERT_EQUAL(bus256.GetStops()[5], "Biryulyovo Zapadnoye");

  ASSERT_EQUAL(res[1]->Kind(), Commands::BusCommand);
  auto bus750 = static_cast<BusCommand &>(*res[1]);
  ASSERT_EQUAL(bus750.GetNumber(), 750);
  ASSERT(!bus750.IsCircular());
  ASSERT_EQUAL(bus750.GetStops().size(), 3U);
  ASSERT_EQUAL(bus750.GetStops()[0], "Tolstopaltsevo");
  ASSERT_EQUAL(bus750.GetStops()[1], "Marushkino");
  ASSERT_EQUAL(bus750.GetStops()[2], "Rasskazovka");
}