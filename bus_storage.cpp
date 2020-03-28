#include "bus_storage.h"

#include <set>

using namespace std;

BusId BusStorage::Add(const string &bus_name) {
  BusId id = _buses.size();
  _buses.push_back(bus_name);
  _bus_ids.insert({bus_name, id});
  return id;
}

vector<string> BusStorage::GetBusNames(vector<BusId> bus_ids) {
  set<string> res;
  for (auto bus_id : bus_ids) {
    res.insert(GetName(bus_id));
  }
  return {res.begin(), res.end()};
}

optional<BusId> BusStorage::TryFind(const string &bus_name) const {
  auto it = _bus_ids.find(bus_name);
  if (it == _bus_ids.end()) {
    return nullopt;
  }
  return it->second;
}