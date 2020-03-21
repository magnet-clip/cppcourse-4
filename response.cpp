#include "response.h"
using namespace std;

string Responses::BusResponse = "BusResponse";

string BusResponse::ToString() const {
  ostringstream os;
  os << "Bus " << bus_number << ": " << num_stops << " stops on route, "
     << num_unique_stops << " unique stops, " << length << " route length";
  return os.str();
}