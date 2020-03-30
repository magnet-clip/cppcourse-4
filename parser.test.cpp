#include "parser.test.h"
#include "command.h"
#include "geomath.h"
#include "io.h"
#include "parser.h"
#include "query.h"
#include "test_runner.h"

#include <cmath>
#include <sstream>

using namespace std;

void TestJsonStopParsing() {
  stringstream s;
  s << "{\"type\":\"Stop\",\"road_distances\":{\"Marushkino\":3900},"
       "\"longitude\":37.20829,\"name\":\"Tolstopaltsevo\","
       "\"latitude\":55.611087}";
  JsonIo io;
  auto cmd = io.ReadCommand(s);
  ASSERT_EQUAL(cmd->Kind(), Commands::StopCommand);
  auto &cmd_stop = static_cast<StopCommand &>(*cmd);
  GeoPoint expected{Latitude(55.611087), Longitude(37.20829)};
  ASSERT_EQUAL(cmd_stop.GetName(), "Tolstopaltsevo");
  ASSERT(fabs(cmd_stop.GetLocation().GetLatitude() - expected.GetLatitude()) <
         0.000001);
  ASSERT(fabs(cmd_stop.GetLocation().GetLongitude() - expected.GetLongitude()) <
         0.000001);

  auto distances = cmd_stop.GetDistances();
  ASSERT_EQUAL(distances.size(), 1UL);
  ASSERT_EQUAL(distances.at("Marushkino"), 3900.0);
}

void TestStopCommandParsing() {
  {
    stringstream cmd_stream;
    cmd_stream << "1" << endl
               << "Stop Tolstopaltsevo: 55.611087, 37.20829" << endl;
    StringIo io;
    auto res = io.ReadCommands(cmd_stream);
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
    StringIo io;
    auto res = io.ReadCommands(cmd_stream);
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
      << "3" << endl
      << "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo "
         "Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"
      << endl
      << "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka" << endl
      << "Bus 1001: Krekshino" << endl;
  StringIo io;
  auto res = io.ReadCommands(cmd_stream);
  ASSERT_EQUAL(res.size(), 3U);

  ASSERT_EQUAL(res[0]->Kind(), Commands::BusCommand);
  auto bus256 = static_cast<BusCommand &>(*res[0]);
  ASSERT_EQUAL(bus256.GetName(), "256");
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
  ASSERT_EQUAL(bus750.GetName(), "750");
  ASSERT(!bus750.IsCircular());
  ASSERT_EQUAL(bus750.GetStops().size(), 3U);
  ASSERT_EQUAL(bus750.GetStops()[0], "Tolstopaltsevo");
  ASSERT_EQUAL(bus750.GetStops()[1], "Marushkino");
  ASSERT_EQUAL(bus750.GetStops()[2], "Rasskazovka");

  ASSERT_EQUAL(res[2]->Kind(), Commands::BusCommand);
  auto bus1001 = static_cast<BusCommand &>(*res[2]);
  ASSERT_EQUAL(bus1001.GetName(), "1001");
  ASSERT_EQUAL(bus1001.GetStops().size(), 1U);
  ASSERT_EQUAL(bus1001.GetStops()[0], "Krekshino");
}

CommandPtr ReadCommandFromString(const string &str) {
  stringstream s;
  s << str;
  StringIo io;
  return io.ReadCommand(s);
}

void TestStopCommandParsingExtended() {
  {
    const auto x =
        ReadCommandFromString("Stop Rasskazovka: 55.632761, 37.333324");
    const auto &stop = static_cast<const StopCommand &>(*x);

    ASSERT_EQUAL(stop.GetName(), "Rasskazovka");

    GeoPoint expected_location{Latitude(55.632761), Longitude(37.333324)};
    ASSERT_EQUAL(stop.GetLocation(), expected_location);

    auto &distances = stop.GetDistances();
    ASSERT_EQUAL(distances.size(), 0UL);
  }
  {
    const auto x = ReadCommandFromString(
        "Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino");
    const auto &stop = static_cast<const StopCommand &>(*x);

    ASSERT_EQUAL(stop.GetName(), "Tolstopaltsevo");

    GeoPoint expected_location{Latitude(55.611087), Longitude(37.20829)};
    ASSERT_EQUAL(stop.GetLocation(), expected_location);

    auto distances = stop.GetDistances();
    ASSERT_EQUAL(distances.size(), 1UL);
    ASSERT_EQUAL(distances.at("Marushkino"), 3900.0);
  }
  {
    const auto x = ReadCommandFromString(
        "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to "
        "Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam");
    const auto &stop = static_cast<const StopCommand &>(*x);

    ASSERT_EQUAL(stop.GetName(), "Biryulyovo Zapadnoye");

    GeoPoint expected_location{Latitude(55.574371), Longitude(37.6517)};
    ASSERT_EQUAL(stop.GetLocation(), expected_location);

    auto &distances = stop.GetDistances();
    ASSERT_EQUAL(distances.size(), 3UL);
    ASSERT_EQUAL(distances.at("Rossoshanskaya ulitsa"), 7500.0);
    ASSERT_EQUAL(distances.at("Biryusinka"), 1800.0);
    ASSERT_EQUAL(distances.at("Universam"), 2400.0);
  }
}

void TestBusQueryParsing() {
  stringstream cmd_stream;
  cmd_stream << "3" << endl
             << "Bus 256" << endl
             << "Bus 750" << endl
             << "Bus 751" << endl;
  StringIo io;
  auto res = io.ReadQueries(cmd_stream);
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
