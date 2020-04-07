#include "io.test.h"

#include "io.h"
#include "json.h"
#include "test_runner.h"

#include <iostream>
#include <sstream>
#include <unordered_map>

using namespace std;

void TestJsonSample() {
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

  JsonIo json_io;
  StringIo string_io;
  stringstream os;
  const auto &[settings, commands, queries] = json_io.ReadInput(s);
  Database db;
  db.ExecuteCommands(commands);
  const auto responses = db.ExecuteQueries(queries);
  const auto strings = string_io.ProcessResponses(responses);
  ASSERT_EQUAL(strings[0], "Bus 256: 6 stops on route, 5 unique stops, 5950 "
                           "route length, 1.361239 curvature");
  const auto jsons = json_io.ProcessResponses(responses);
  ASSERT_EQUAL(jsons[0],
               "{\"curvature\":1.361239,\"request_id\":1965312327,\"route_"
               "length\":5950,\"stop_count\":6,\"unique_stop_count\":5}");
  ASSERT_EQUAL(jsons[1],
               "{\"curvature\":1.318084,\"request_id\":519139350,\"route_"
               "length\":27600,\"stop_count\":5,\"unique_stop_count\":3}");
  ASSERT_EQUAL(jsons[2],
               "{\"error_message\":\"not found\",\"request_id\":194217464}");
  ASSERT_EQUAL(jsons[3],
               "{\"error_message\":\"not found\",\"request_id\":746888088}");
  ASSERT_EQUAL(jsons[4], "{\"buses\":[],\"request_id\":65100610}");
  ASSERT_EQUAL(jsons[5],
               "{\"buses\":[\"256\",\"828\"],\"request_id\":1042838872}");
}

void Compare(const unordered_map<RequestId, Json::Node> &actual,
             const unordered_map<RequestId, Json::Node> &expected) {
  ASSERT_EQUAL(actual.size(), expected.size());
  for (const auto &[id, actual_node] : actual) {
    ASSERT_EQUAL(expected.count(id), 1UL);
    ASSERT_EQUAL(actual_node, expected.at(id));
  }
}

unordered_map<RequestId, Json::Node>
ReadExpectedResponsesFromJson(istream &is) {
  auto doc = Json::Load(is);
  unordered_map<RequestId, Json::Node> res;
  for (const auto &node : doc.GetRoot().AsArray()) {
    auto id = node.AsMap().at("request_id").AsInt();
    res.insert({id, node});
  }
  return res;
}

void RunIntegrationTest(istream &input, istream &output) {
  JsonIo json_io;
  stringstream os;
  const auto &[settings, commands, queries] = json_io.ReadInput(input);
  Database db;
  db.UseSettings(settings);
  db.ExecuteCommands(commands);
  const auto responses = db.ExecuteQueries(queries);
  unordered_map<RequestId, Json::Node> implied_responses;
  for (const auto &response : responses) {
    const auto &node = json_io.ResponseToJsonNode(response);
    implied_responses.insert({response->GetId(), node});
  }

  const auto expected_responses = ReadExpectedResponsesFromJson(output);

  Compare(implied_responses, expected_responses);
}