#include "io.h"
using namespace std;

CommandPtr ReadCommand(istream &is, const Parser &parser) {
  string command;
  is >> command;

  string line;
  getline(is, line);
  return parser.ParseCommand(command, line);
}

QueryPtr ReadQuery(istream &is, const Parser &parser) {
  string query;
  is >> query;

  string line;
  getline(is, line);
  return parser.ParseQuery(query, line);
}

vector<CommandPtr> ReadCommands(istream &is, const Parser &parser) {
  int request_count = 0;
  is >> request_count;
  vector<CommandPtr> res;
  for (int i = 0; i < request_count; i++) {
    res.push_back(ReadCommand(is, parser));
  }
  return res;
}

vector<QueryPtr> ReadQueries(istream &is, const Parser &parser) {
  vector<QueryPtr> res;

  int count = 0;
  is >> count;
  for (int i = 0; i < count; i++) {
    res.push_back(ReadQuery(is, parser));
  }
  return res;
}

void InAndOut(istream &is, ostream &os, const Serializer &serializer,
              const Parser &parser) {
  auto commands = ReadCommands(is, parser);
  auto queries = ReadQueries(is, parser);

  Database db;
  db.ExecuteCommands(commands);
  auto res = db.ExecuteQueries(queries);
  for (const auto &out : res) {
    os << serializer.Serialize(out) << endl;
  }
}
