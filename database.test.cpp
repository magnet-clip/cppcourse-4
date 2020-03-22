#include "database.test.h"
#include "database.h"
#include "test_runner.h"

#include <sstream>
#include <string>
#include <vector>

using namespace std;

void TestSample() {
  stringstream s;
  s << "10" << endl
    << "Stop Tolstopaltsevo: 55.611087, 37.20829" << endl
    << "Stop Marushkino: 55.595884, 37.209755" << endl
    << "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo "
       "Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"
    << endl
    << "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka" << endl
    << "Stop Rasskazovka: 55.632761, 37.333324" << endl
    << "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517" << endl
    << "Stop Biryusinka: 55.581065, 37.64839" << endl
    << "Stop Universam: 55.587655, 37.645687" << endl
    << "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656" << endl
    << "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164" << endl
    << "3" << endl
    << "Bus 256" << endl
    << "Bus 750" << endl
    << "Bus 751" << endl;

  auto commands = ReadCommands(s);
  auto queries = ReadQueries(s);

  ASSERT_EQUAL(commands.size(), 10U);
  ASSERT_EQUAL(queries.size(), 3U);

  vector<string> expected{
      "Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length",
      "Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length",
      "Bus 751: not found"};

  Database db;
  db.ExecuteCommands(commands);
  auto responses = db.ExecuteQueries(queries);
  vector<string> res;
  for (const auto &response : responses) {
    res.push_back(response->ToString());
  }
  ASSERT_EQUAL(res, expected);
}

void TestSample2() {
  stringstream s;
  s << "13" << endl
    << "Stop Tolstopaltsevo: 55.611087, 37.20829" << endl
    << "Stop Marushkino: 55.595884, 37.209755" << endl
    << "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo "
       "Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye"
    << endl
    << "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka" << endl
    << "Stop Rasskazovka: 55.632761, 37.333324" << endl
    << "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517" << endl
    << "Stop Biryusinka: 55.581065, 37.64839" << endl
    << "Stop Universam: 55.587655, 37.645687" << endl
    << "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656" << endl
    << "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164" << endl
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

  auto commands = ReadCommands(s);
  auto queries = ReadQueries(s);

  ASSERT_EQUAL(commands.size(), 13U);
  ASSERT_EQUAL(queries.size(), 6U);

  vector<string> expected{
      "Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length",
      "Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length",
      "Bus 751: not found",
      "Stop Samara: not found",
      "Stop Prazhskaya: no buses",
      "Stop Biryulyovo Zapadnoye: buses 256 828"};

  Database db;
  db.ExecuteCommands(commands);

  auto responses = db.ExecuteQueries(queries);
  vector<string> res;
  for (const auto &response : responses) {
    res.push_back(response->ToString());
  }
  ASSERT_EQUAL(res, expected);
}