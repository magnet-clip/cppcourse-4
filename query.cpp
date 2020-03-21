#include "query.h"

#include <sstream>
#include <string>

using namespace std;

string Queries::BusQuery = "Bus";

BusQuery::BusQuery(string_view line) { _number = stoi(string(line)); }

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

vector<shared_ptr<Query>> ReadQueries(istream &is) {
  vector<shared_ptr<Query>> res;

  int count = 0;
  is >> count;
  for (int i = 0; i < count; i++) {
    string query;
    is >> query;

    string line;
    getline(is, line);
    if (query == Queries::BusQuery) {
      res.push_back(make_shared<BusQuery>(line));
    }
  }
  return res;
}
