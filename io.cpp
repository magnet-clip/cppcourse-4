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

string GetCommandFromJson(istream &is) {}

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

void StringIo::InAndOut(istream &is, ostream &os) {
  auto commands = ReadCommands(is);
  auto queries = ReadQueries(is);

  Database db;
  db.ExecuteCommands(commands);
  auto res = db.ExecuteQueries(queries);
  for (const auto &out : res) {
    os << Serialize(out) << endl;
  }
}

vector<CommandPtr> JsonIo::ReadCommands(istream &is) {}
vector<QueryPtr> JsonIo::ReadQueries(istream &is) {}

void JsonIo::InAndOut(istream &is, ostream &os) {}