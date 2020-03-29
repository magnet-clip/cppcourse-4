#include "command.h"
#include "str_utils.h"

using namespace std;

string Commands::StopCommand = "Stop";
string Commands::BusCommand = "Bus";

bool BusCommand::operator==(const Command &other) const {
  if (Kind() != other.Kind()) {
    return false;
  }
  auto &other_cmd = static_cast<const BusCommand &>(other);
  // not comparing stops!
  return _name == other_cmd._name;
}

bool StopCommand::operator==(const Command &other) const {
  if (Kind() != other.Kind()) {
    return false;
  }
  auto &other_cmd = static_cast<const StopCommand &>(other);
  return _location == other_cmd._location;
}
