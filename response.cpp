#include "response.h"

#include <iomanip>

using namespace std;

string Responses::FoundBusResponse = "FoundBusResponse";
string Responses::NoBusResponse = "NoBusResponse";
string Responses::FoundStopResponse = "FoundStopResponse";
string Responses::NoStopResponse = "NoStopResponse";

string FoundBusResponse::ToString() const {
  ostringstream os;
  os.precision(7);
  os << "Bus " << bus_number << ": " << num_stops << " stops on route, "
     << num_unique_stops << " unique stops, " << length << " route length, "
     << curvature << " curvature";
  return os.str();
}

string NoBusResponse::ToString() const {
  ostringstream os;
  os << "Bus " << bus_number << ": not found";
  return os.str();
}

string FoundStopResponse::ToString() const {
  ostringstream os;
  os << "Stop " << stop_name << ": ";
  if (bus_names.size() == 0) {
    os << "no buses";
  } else {
    os << "buses";
    for (const auto &bus_name : bus_names) {
      os << " " << bus_name;
    }
  }
  return os.str();
}

string NoStopResponse::ToString() const {
  ostringstream os;
  os << "Stop " << stop_name << ": not found";
  return os.str();
}