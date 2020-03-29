#include "stop_storage.h"

using namespace std;

StopId StopStorage::AddQualifiedStop(const string &stop_name,
                                     GeoPoint location) {
  const auto it = _stop_ids.find(stop_name);
  if (it != _stop_ids.end()) {
    auto id = it->second;
    const auto stop_ptr = make_shared<QualifiedStop>(stop_name, location);
    _stops[id] = stop_ptr;
    return id;
  } else {
    const auto stop_ptr = make_shared<QualifiedStop>(stop_name, location);
    _stops.push_back(stop_ptr);
    auto id = _stops.size() - 1;
    _stop_ids.insert({stop_name, id});
    return id;
  }
}

StopId StopStorage::GetOrAddUnqualified(const string &stop_name) {
  const auto it = _stop_ids.find(stop_name);
  if (it == _stop_ids.end()) {
    const auto stop_ptr = make_shared<Stop>(stop_name);
    _stops.push_back(stop_ptr);
    auto id = _stops.size() - 1;
    _stop_ids.insert({stop_name, id});
    return id;
  } else {
    return it->second;
  }
}

vector<StopId>
StopStorage::GetOrAddUnqualifiedBulk(const vector<string> &stop_names) {
  vector<StopId> res;
  for (const auto &stop_name : stop_names) {
    res.push_back(GetOrAddUnqualified(stop_name));
  }
  return res;
}

optional<StopId> StopStorage::TryFindIdByName(const string &stop_name) const {
  const auto it = _stop_ids.find(stop_name);
  if (it != _stop_ids.end()) {
    return it->second;
  }
  return nullopt;
}

StopPtr StopStorage::TryFindByName(const string &stop_name) const {
  const auto it = _stop_ids.find(stop_name);
  if (it != _stop_ids.end()) {
    return _stops[it->second];
  }
  return nullptr;
}