#include "command.h"

using namespace std;
string Commands::StopCommand = "Stop";
string Commands::BusCommand = "Bus";

StopCommand::StopCommand(string_view line) {
  int first_non_space = line.find_first_not_of(" \r\n\t");
  line.remove_prefix(first_non_space);
  int colon_pos = line.find(':');
  _name = string(line.substr(0, colon_pos));
  line.remove_prefix(colon_pos + 1);

  int comma_pos = line.find(',');
  double latitude = stod(string(line.substr(0, comma_pos)));
  line.remove_prefix(comma_pos + 1);
  double longitude = stod(string(line));
  _location = GeoPoint(Latitude(latitude), Longitude(longitude));
}

string StopCommand::ToString() const {
  ostringstream os;
  os << Kind() << " [" << _name << "] " << GetLocation();
  return os.str();
}

BusCommand::BusCommand(string_view line) {}

bool BusCommand::operator==(const Command &other) const {
  if (Kind() != other.Kind()) {
    return false;
  }
  auto &other_cmd = static_cast<const BusCommand &>(other);
  // not comparing stops!
  return _number == other_cmd._number;
}

string BusCommand::ToString() const {
  ostringstream os;
  os << Kind() << " [" << _number << "] (";
  bool first = true;
  for (const auto &stop : _stops) {
    if (!first) {
      os << ", ";
    } else {
      first = false;
    }
    os << stop;
  }
  os << ")";
  return os.str();
}

bool StopCommand::operator==(const Command &other) const {
  if (Kind() != other.Kind()) {
    return false;
  }
  auto &other_cmd = static_cast<const StopCommand &>(other);
  return _location == other_cmd._location;
}

vector<shared_ptr<Command>> ReadCommands(istream &is) {
  int request_count = 0;
  is >> request_count;
  vector<shared_ptr<Command>> res;
  for (int i = 0; i < request_count; i++) {
    string command;
    is >> command;

    string line;
    getline(is, line);
    if (command == Commands::StopCommand) {
      res.push_back(make_shared<StopCommand>(line));
    } else if (command == Commands::BusCommand) {
      res.push_back(make_shared<BusCommand>(line));
    }
  }
  return res;
}