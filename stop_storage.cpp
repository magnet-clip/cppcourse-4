#include "stop_storage.h"

using namespace std;

StopId StopStorage::AddQualifiedStop(const string &stop_name,
                                     GeoPoint location) {
  const auto it = _stop_ids.find(stop_name);
  if (it != _stop_ids.end()) {
    auto stop_id = it->second;
    const auto stop_ptr = make_shared<QualifiedStop>(stop_id, stop_name, location);
    _stops[stop_id] = stop_ptr;
    return stop_id;
  } else {
    auto stop_id = _stops.size();
    const auto stop_ptr = make_shared<QualifiedStop>(stop_id, stop_name, location);
    _stops.push_back(stop_ptr);
    _stop_ids.insert({stop_name, stop_id});
    return stop_id;
  }
}

StopId StopStorage::GetOrAddUnqualified(const string &stop_name) {
  const auto it = _stop_ids.find(stop_name);
  if (it == _stop_ids.end()) {
    auto stop_id = _stops.size();
    const auto stop_ptr = make_shared<Stop>(stop_id, stop_name);
    _stops.push_back(stop_ptr);
    _stop_ids.insert({stop_name, stop_id});
    return stop_id;
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