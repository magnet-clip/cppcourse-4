#include "bus_storage.h"

using namespace std;

BusId BusStorage::Add(const string &bus_name) {
  BusId id = _buses.size();
  _buses.push_back({id, bus_name});
  return id;
}
