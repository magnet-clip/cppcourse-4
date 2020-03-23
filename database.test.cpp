#include "database.test.h"
#include "database.h"
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
  auto commands = ReadCommands(s);
  auto queries = ReadQueries(s);

  Database db;
  db.ExecuteCommands(commands);

  DISTANCE("Tolstopaltsevo", "Marushkino", 3900.0);
  DISTANCE("Marushkino", "Rasskazovka", 9900.0);
  DISTANCE("Biryulyovo Zapadnoye", "Rossoshanskaya ulitsa", 7500.0);
  DISTANCE("Biryulyovo Zapadnoye", "Biryusinka", 1800.0);
  DISTANCE("Biryulyovo Zapadnoye", "Universam", 2400.0);
  DISTANCE("Biryusinka", "Universam", 750.0);

  auto responses = db.ExecuteQueries(queries);
  vector<string> res;
  for (const auto &response : responses) {
    res.push_back(response->ToString());
  }
  ASSERT_EQUAL(res, expected);
}