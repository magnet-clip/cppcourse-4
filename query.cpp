#include "query.h"
#include "str_utils.h"

#include <sstream>
#include <string>

using namespace std;

string Queries::BusQuery = "Bus";
string Queries::StopQuery = "Stop";

bool BusQuery::operator==(const Query &other) const {
  if (other.Kind() != Kind()) {
    return false;
  }
  auto &other_qry = static_cast<const BusQuery &>(other);
  return _name == other_qry._name;
}

bool StopQuery::operator==(const Query &other) const {
  if (other.Kind() != Kind()) {
    return false;
  }
  auto &other_qry = static_cast<const StopQuery &>(other);
  return _name == other_qry._name;
}
