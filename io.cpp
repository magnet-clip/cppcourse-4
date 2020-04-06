#include "io.h"
#include "json.h"

using namespace std;

CommandPtr StringIo::ReadCommand(istream &is) {
  string command;
  is >> command;

  string line;
  getline(is, line);
  _parser.SetLine(line);

  return _parser.ParseCommand(command);
}

QueryPtr StringIo::ReadQuery(istream &is) {
  string query;
  is >> query;

  string line;
  getline(is, line);
  _parser.SetLine(line);

  return _parser.ParseQuery(query);
}

vector<CommandPtr> StringIo::ReadCommands(istream &is) {
  int request_count = 0;
  is >> request_count;
  vector<CommandPtr> res;
  for (int i = 0; i < request_count; i++) {
    res.push_back(ReadCommand(is));
  }
  return res;
}

vector<QueryPtr> StringIo::ReadQueries(istream &is) {
  vector<QueryPtr> res;

  int count = 0;
  is >> count;
  for (int i = 0; i < count; i++) {
    res.push_back(ReadQuery(is));
  }
  return res;
}

Input StringIo::ReadInput(std::istream &is) {
  return {{0, 0}, ReadCommands(is), ReadQueries(is)};
}

CommandPtr JsonIo::ReadCommand(istream &is) {
  using Json::Document;
  Document doc = Json::Load(is);
  auto node = doc.GetRoot();
  auto info = node.AsMap();
  auto command = info.at("type").AsString();
  _parser.SetNode(node);
  return _parser.ParseCommand(command);
}

QueryPtr JsonIo::ReadQuery(istream &is) {
  using Json::Document;
  Document doc = Json::Load(is);
  auto node = doc.GetRoot();
  auto info = node.AsMap();
  auto query = info.at("type").AsString();
  return _parser.ParseQuery(query);
}

Input JsonIo::ReadInput(std::istream &is) {
  auto doc = Json::Load(is);
  auto nodes = doc.GetRoot();

  // If routing_settings are missing, I set them to zero
  RoutingSettings settings{0, 0};
  const auto &nodes_map = nodes.AsMap();
  if (nodes_map.count("routing_settings")) {
    auto incoming_settings = nodes_map.at("routing_settings").AsMap();
    settings = {
        incoming_settings.at("bus_wait_time").AsInt(),
        static_cast<int>(incoming_settings.at("bus_velocity").AsNumber())};
  }
  auto incoming_commands = nodes_map.at("base_requests").AsArray();
  vector<CommandPtr> commands;
  for (const auto command : incoming_commands) {
    _parser.SetNode(command);
    commands.push_back(
        _parser.ParseCommand(command.AsMap().at("type").AsString()));
  }
  vector<QueryPtr> queries;
  auto incoming_queries = nodes_map.at("stat_requests").AsArray();
  for (const auto query : incoming_queries) {
    _parser.SetNode(query);
    queries.push_back(_parser.ParseQuery(query.AsMap().at("type").AsString()));
  }
  return {move(settings), move(commands), move(queries)};
}

void InAndOut(istream &is, ostream &os, Io &io) {
  const auto &[settings, commands, queries] = io.ReadInput(is);
  Database db;

  db.ExecuteCommands(commands);
  const auto &res = db.ExecuteQueries(queries);
  const auto &output = io.ProcessResponses(res);
  bool first = true;
  os << "[";
  for (const auto &out : output) {
    if (first) {
      first = false;
    } else {
      os << "," << endl;
    }
    os << out;
  }
  os << "]";
}

Json::Node JsonIo::ResponseToJsonNode(ResponsePtr response) {
  if (response->Kind() == Responses::NoBusResponse) {
    return GetJsonNode(static_cast<NoBusResponse &>(*response));
  } else if (response->Kind() == Responses::FoundBusResponse) {
    return GetJsonNode(static_cast<FoundBusResponse &>(*response));
  } else if (response->Kind() == Responses::NoStopResponse) {
    return GetJsonNode(static_cast<NoStopResponse &>(*response));
  } else if (response->Kind() == Responses::FoundStopResponse) {
    return GetJsonNode(static_cast<FoundStopResponse &>(*response));
  } else {
    throw std::invalid_argument(response->Kind());
  }
}
