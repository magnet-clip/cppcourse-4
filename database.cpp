#include "database.h"

using namespace std;

Database::Database(const vector<CommandPtr> &commands) {
  for (const auto &command_ptr : commands) {
    if (command_ptr->Kind() == Commands::BusCommand) {
      ExecuteBusCommand(static_cast<const BusCommand &>(*command_ptr));
    } else if (command_ptr->Kind() == Commands::StopCommand) {
      ExecuteStopCommand(static_cast<const StopCommand &>(*command_ptr));
    }
  }
}

void Database::ExecuteBusCommand(const BusCommand &command) {
  Route route(command.IsCircular(), command.GetStops());
  _routes.insert({command.GetNumber(), route});
}

void Database::ExecuteStopCommand(const StopCommand &command) {
  _stops[command.GetName()] = {command.GetLocation(), command.GetName()};
}

unique_ptr<BusResponse> Database::ExecuteBusQuery(const BusQuery &query) {
  auto number = query.GetNumber();
  if (_routes.count(number)) {
    FoundBusResponse response;
    auto &route = _routes.at(number);
    return make_unique<FoundBusResponse>(response);
  } else {
    return make_unique<NoBusResponse>(number);
  }
}