#include "route_item.h"

using namespace std;

ostream &operator<<(ostream &ss, const BusRouteItem &bus_item) {
  return ss << "Bus " << bus_item.bus_name << ", " << bus_item.span_count << " stops";
}

ostream &operator<<(ostream &ss, const WaitRouteItem &wait_item) {
  return ss << "Wait on " << wait_item.stop_name;
}