#pragma once

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct Record {
  string id;
  string title;
  string user;
  int timestamp;
  int karma;
};

bool operator==(const Record &a, const Record &b) {
  return tie(a.id, a.title, a.user, a.timestamp, a.karma) ==
         tie(b.id, b.title, b.user, b.timestamp, b.karma);
}

class Database {
 public:
  bool Put(const Record &record) {
    auto [it, success] = _records.insert({record.id, record});
    if (success) {
      auto &record_ref = it->second;
      if (_records_by_timestamp[record.timestamp].count(record_ref.id) > 0) {
        throw invalid_argument("timestamp");
      }
      _records_by_timestamp[record.timestamp].insert(record_ref.id);

      if (_records_by_karma[record.karma].count(record_ref.id) > 0) {
        throw invalid_argument("karma");
      }
      _records_by_karma[record.karma].insert(record_ref.id);
      // TODO use recordref ???
      if (_records_by_user[record.user].count(record_ref.id) > 0) {
        throw invalid_argument("user");
      }
      _records_by_user[record.user].insert(record_ref.id);
    }
    return success;
  }

  const Record *GetById(const string &id) const {
    auto it = _records.find(id);
    if (it == _records.end()) {
      return nullptr;
    } else {
      const Record &ref_to_record = it->second;
      return &ref_to_record;
    }
  }

  bool Erase(const string &id) {
    auto it = _records.find(id);
    if (it == _records.end()) {
      return false;
    }

    const Record &ref_to_record = it->second;

    {
      auto it = _records_by_timestamp.find(ref_to_record.timestamp);
      if (it != _records_by_timestamp.end()) {
        it->second.erase(id);
      }
    }
    {
      auto it = _records_by_karma.find(ref_to_record.karma);
      if (it != _records_by_karma.end()) {
        it->second.erase(id);
      }
    }

    {
      auto it = _records_by_user.find(ref_to_record.user);
      if (it != _records_by_user.end()) {
        it->second.erase(id);
      }
    }
    _records.erase(it);
    return true;
  }

  template <typename Callback>
  void RangeByTimestamp(int low, int high, Callback callback) const {
    if (low > high) return;
    auto start = _records_by_timestamp.lower_bound(low);
    auto end = _records_by_timestamp.upper_bound(high);
    for (auto &it = start; it != end; it++) {
      for (auto id : it->second) {
        if (!callback(*GetById(string(id)))) {
          return;
        }
      }
    }
  }

  template <typename Callback>
  void RangeByKarma(int low, int high, Callback callback) const {
    if (low > high) return;
    auto start = _records_by_karma.lower_bound(low);
    auto end = _records_by_karma.upper_bound(high);
    for (auto it = start; it != end; it++) {
      for (auto id : it->second) {
        if (!callback(*GetById(string(id)))) return;
      }
    }
  }

  template <typename Callback>
  void AllByUser(const string &user, Callback callback) const {
    auto pos = _records_by_user.find(user);
    if (pos != _records_by_user.end()) {
      for (auto it = pos->second.begin(); it != pos->second.end(); it++) {
        if (!callback(*GetById(string(*it)))) return;
      }
    }
  }

 private:
  unordered_map<string, Record> _records;
  map<int, unordered_set<string_view>> _records_by_timestamp;
  map<int, unordered_set<string_view>> _records_by_karma;
  unordered_map<string, unordered_set<string_view>> _records_by_user;
};