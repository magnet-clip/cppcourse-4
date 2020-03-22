#include "response.h"

#include <iomanip>

using namespace std;

string Responses::FoundBusResponse = "FoundBusResponse";
string Responses::NoBusResponse = "NoBusResponse";
string Responses::FoundStopResponse = "FoundStopResponse";
string Responses::NoStopResponse = "NoStopResponse";

string FoundBusResponse::ToString() const {
  ostringstream os;
  os << "Bus " << bus_number << ": " << num_stops << " stops on route, "
     << num_unique_stops << " unique stops, " << setprecision(6) << length
     << " route length";
  return os.str();
}

string NoBusResponse::ToString() const {
  ostringstream os;
  os << "Bus " << bus_number << ": not found";
  return os.str();
}