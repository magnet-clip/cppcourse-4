#include "query.h"
#include "str_utils.h"

#include <sstream>
#include <string>

using namespace std;

string Queries::BusQuery = "Bus";
string Queries::StopQuery = "Stop";

BusQuery::BusQuery(string_view line) {
  RemoveLeadingSpaces(line);
  _number = string(line);
}

string BusQuery::ToString() const {
  ostringstream os;
  os << Kind() << " [" << _number << "]";
  return os.str();
}

bool BusQuery::operator==(const Query &other) const {
  if (other.Kind() != Kind()) {
    return false;
  }
  auto &other_qry = static_cast<const BusQuery &>(other);
  return _number == other_qry._number;
}

StopQuery::StopQuery(string_view line) {
  RemoveLeadingSpaces(line);
  _name = string(line);
}

string StopQuery::ToString() const {
  ostringstream os;
  os << Kind() << " [" << _name << "]";
  return os.str();
}

bool StopQuery::operator==(const Query &other) const {
  if (other.Kind() != Kind()) {
    return false;
  }
  auto &other_qry = static_cast<const StopQuery &>(other);
  return _name == other_qry._name;
}

vector<QueryPtr> ReadQueries(istream &is) {
  vector<QueryPtr> res;

  int count = 0;
  is >> count;
  for (int i = 0; i < count; i++) {
    string query;
    is >> query;

    string line;
    getline(is, line);
    if (query == Queries::BusQuery) {
      res.push_back(make_shared<BusQuery>(line));
    } else if (query == Queries::StopQuery) {
      res.push_back(make_shared<StopQuery>(line));
    }
  }
  return res;
}
