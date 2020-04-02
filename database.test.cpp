#include "database.test.h"
#include "database.h"
#include "io.h"
#include "serializer.h"
#include "test_runner.h"

#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define DISTANCE(a, b, d) ASSERT_EQUAL(db.GetStopDistance(a, b).value(), d);

void TestSample3() {
  stringstream s;
  s << "13" << endl
    << "Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino" << endl
    << "Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka" << endl
    << "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo "
       "Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"
    << endl
    << "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka" << endl
    << "Stop Rasskazovka: 55.632761, 37.333324" << endl
    << "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to "
       "Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam"
    << endl
    << "Stop Biryusinka: 55.581065, 37.64839, 750m to Universam" << endl
    << "Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya "
       "ulitsa, 900m to Biryulyovo Tovarnaya"
    << endl
    << "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo "
       "Passazhirskaya"
    << endl
    << "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to "
       "Biryulyovo Zapadnoye"
    << endl
    << "Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > "
       "Biryulyovo Zapadnoye"
    << endl
    << "Stop Rossoshanskaya ulitsa: 55.595579, 37.605757" << endl
    << "Stop Prazhskaya: 55.611678, 37.603831" << endl
    << "6" << endl
    << "Bus 256" << endl
    << "Bus 750" << endl
    << "Bus 751" << endl
    << "Stop Samara" << endl
    << "Stop Prazhskaya" << endl
    << "Stop Biryulyovo Zapadnoye" << endl;

  vector<string> expected{
      "Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.361239 "
      "curvature",
      "Bus 750: 5 stops on route, 3 unique stops, 27600 route length, 1.318084 "
      "curvature",
      "Bus 751: not found",
      "Stop Samara: not found",
      "Stop Prazhskaya: no buses",
      "Stop Biryulyovo Zapadnoye: buses 256 828",
  };
  StringIo io;
  const auto &[commands, queries] = io.ReadCommandsAndQueries(s);

  Database db;
  db.ExecuteCommands(commands);

  DISTANCE("Tolstopaltsevo", "Marushkino", 3900.0);
  DISTANCE("Marushkino", "Rasskazovka", 9900.0);
  DISTANCE("Biryulyovo Zapadnoye", "Rossoshanskaya ulitsa", 7500.0);
  DISTANCE("Biryulyovo Zapadnoye", "Biryusinka", 1800.0);
  DISTANCE("Biryulyovo Zapadnoye", "Universam", 2400.0);
  DISTANCE("Biryusinka", "Universam", 750.0);
  auto responses = db.ExecuteQueries(queries);
  ASSERT_EQUAL(responses.size(), expected.size());
  vector<string> res = io.ProcessResponses(responses);
  ASSERT_EQUAL(res[0], expected[0]);
  ASSERT_EQUAL(res[1], expected[1]);
  ASSERT_EQUAL(res[2], expected[2]);
  ASSERT_EQUAL(res[3], expected[3]);
  ASSERT_EQUAL(res[4], expected[4]);
  ASSERT_EQUAL(res[5], expected[5]);
  ASSERT_EQUAL(res.size(), expected.size());
}

void TestJsonCommandsQueries() {
  stringstream s;
  s << "{\"base_requests\":[{\"type\":\"Stop\",\"road_distances\":{"
       "\"Marushkino\":3900},\"longitude\":37.20829,\"name\":"
       "\"Tolstopaltsevo\","
       "\"latitude\":55.611087},{\"type\":\"Stop\",\"road_distances\":{"
       "\"Rasskazovka\":9900},\"longitude\":37.209755,\"name\":\"Marushkino\","
       "\"latitude\":55.595884},{\"type\":\"Bus\",\"name\":\"256\",\"stops\":["
       "\"Biryulyovo Zapadnoye\",\"Biryusinka\",\"Universam\",\"Biryulyovo "
       "Tovarnaya\",\"Biryulyovo Passazhirskaya\",\"Biryulyovo "
       "Zapadnoye\"],\"is_roundtrip\":true},{\"type\":\"Bus\",\"name\":\"750\","
       "\"stops\":[\"Tolstopaltsevo\",\"Marushkino\",\"Rasskazovka\"],\"is_"
       "roundtrip\":false},{\"type\":\"Stop\",\"road_distances\":{},"
       "\"longitude\":37.333324,\"name\":\"Rasskazovka\",\"latitude\":55."
       "632761},"
       "{\"type\":\"Stop\",\"road_distances\":{\"Rossoshanskaya "
       "ulitsa\":7500,\"Biryusinka\":1800,\"Universam\":2400},\"longitude\":37."
       "6517,\"name\":\"Biryulyovo "
       "Zapadnoye\",\"latitude\":55.574371},{\"type\":\"Stop\",\"road_"
       "distances\":{\"Universam\":750},\"longitude\":37.64839,\"name\":"
       "\"Biryusinka\",\"latitude\":55.581065},{\"type\":\"Stop\",\"road_"
       "distances\":{\"Rossoshanskaya ulitsa\":5600,\"Biryulyovo "
       "Tovarnaya\":900},\"longitude\":37.645687,\"name\":\"Universam\","
       "\"latitude\":55.587655},{\"type\":\"Stop\",\"road_distances\":{"
       "\"Biryulyovo "
       "Passazhirskaya\":1300},\"longitude\":37.653656,\"name\":\"Biryulyovo "
       "Tovarnaya\",\"latitude\":55.592028},{\"type\":\"Stop\",\"road_"
       "distances\":{\"Biryulyovo "
       "Zapadnoye\":1200},\"longitude\":37.659164,\"name\":\"Biryulyovo "
       "Passazhirskaya\",\"latitude\":55.580999},{\"type\":\"Bus\",\"name\":"
       "\"828\",\"stops\":[\"Biryulyovo "
       "Zapadnoye\",\"Universam\",\"Rossoshanskaya ulitsa\",\"Biryulyovo "
       "Zapadnoye\"],\"is_roundtrip\":true},{\"type\":\"Stop\",\"road_"
       "distances\":{},\"longitude\":37.605757,\"name\":\"Rossoshanskaya "
       "ulitsa\",\"latitude\":55.595579},{\"type\":\"Stop\",\"road_distances\":"
       "{}"
       ",\"longitude\":37.603831,\"name\":\"Prazhskaya\",\"latitude\":55."
       "611678}]"
       ",\"stat_requests\":[{\"type\":\"Bus\",\"name\":\"256\",\"id\":"
       "1965312327}"
       ",{\"type\":\"Bus\",\"name\":\"750\",\"id\":519139350},{\"type\":"
       "\"Bus\","
       "\"name\":\"751\",\"id\":194217464},{\"type\":\"Stop\",\"name\":"
       "\"Samara\",\"id\":746888088},{\"type\":\"Stop\",\"name\":"
       "\"Prazhskaya\","
       "\"id\":65100610},{\"type\":\"Stop\",\"name\":\"Biryulyovo "
       "Zapadnoye\",\"id\":1042838872}]}";

  JsonIo io;
  stringstream os;
  const auto &[commands, queries] = io.ReadCommandsAndQueries(s);
  Database db;
  db.ExecuteCommands(commands);
  const auto responses = db.ExecuteQueries(queries);
  // TODO check responses
  const auto output = io.ProcessResponses(responses);
  for (const auto str : output) {
    // TODO compare
  }
}