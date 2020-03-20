#include "command.h"

using namespace std;
string Commands::StopCommand = "Stop";

string StopCommand::ToString() const {
  ostringstream os;
  os << Kind() << " [" << _name << "] (" << _location.latitude << ", "
     << _location.longitude << ")";
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

    if (command == Commands::StopCommand) {
      string name;
      is >> name;
      GeoPoint location;
      is >> location;
      res.push_back(make_shared<StopCommand>(name, location));
    }
  }
  return res;
}