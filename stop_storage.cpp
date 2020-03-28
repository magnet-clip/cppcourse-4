#include "stop_storage.h"

using namespace std;

StopPtr StopStorage::AddQualifiedStop(const string &stop_name,
                                      GeoPoint location) {
  const auto stop_ptr = make_shared<QualifiedStop>(stop_name, location);
  _stops[stop_name] = stop_ptr;
  return stop_ptr;
}

StopPtr StopStorage::GetOrAddUnqualified(const string &stop_name) {
  const auto it = _stops.find(stop_name);
  if (it == _stops.end()) {
    const auto stop_ptr = make_shared<Stop>(stop_name);
    _stops.insert({stop_name, stop_ptr});
    return stop_ptr;
  } else {
    return it->second;
  }
}

StopPtr StopStorage::TryGet(const string &stop_name) const {
  const auto it = _stops.find(stop_name);
  if (it != _stops.end()) {
    return it->second;
  }
  return nullptr;
}