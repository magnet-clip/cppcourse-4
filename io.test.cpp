#include "io.test.h"
#include "io.h"

#include <iostream>
#include <sstream>

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

  JsonIo io;
  stringstream os;
  const auto &[commands, queries] = io.ReadCommandsAndQueries(s);
  // TODO finish test
}