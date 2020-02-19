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
      _records_by_timestamp[record.timestamp].insert(&record_ref);
      _records_by_karma[record.karma].insert(&record_ref);
      _records_by_user[record.user].insert(&record_ref);
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

    Record &ref_to_record = it->second;

    {
      auto it = _records_by_timestamp.find(ref_to_record.timestamp);
      if (it != _records_by_timestamp.end()) {
        it->second.erase(&ref_to_record);
      }
    }
    {
      auto it = _records_by_karma.find(ref_to_record.karma);
      if (it != _records_by_karma.end()) {
        it->second.erase(&ref_to_record);
      }
    }

    {
      auto it = _records_by_user.find(ref_to_record.user);
      if (it != _records_by_user.end()) {
        it->second.erase(&ref_to_record);
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
      for (auto item : it->second) {
        if (!callback(*item)) {
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
      for (auto item : it->second) {
        if (!callback(*item)) return;
      }
    }
  }

  template <typename Callback>
  void AllByUser(const string &user, Callback callback) const {
    auto pos = _records_by_user.find(user);
    if (pos != _records_by_user.end()) {
      for (auto it = pos->second.begin(); it != pos->second.end(); it++) {
        if (!callback(*(*it))) return;
      }
    }
  }

 private:
  unordered_map<string, Record> _records;
  map<int, unordered_set<Record *>> _records_by_timestamp;
  map<int, unordered_set<Record *>> _records_by_karma;
  unordered_map<string, unordered_set<Record *>> _records_by_user;
};