#include "test_runner.h"

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

// Реализуйте этот класс
class Database {
public:
  bool Put(const Record &record) {
    auto [it, success] = _records.insert({record.id, record});
    if (success) {
      auto &record_ref = it->second;
      _records_by_timestamp.insert({record.timestamp, record_ref.id});
      _records_by_karma.insert({record.karma, record_ref.id});
      _records_by_user.insert({record_ref.user, record_ref.id});
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
      auto [start, stop] =
          _records_by_timestamp.equal_range(ref_to_record.timestamp);
      for (auto &it = start; it != stop;) {
        if (it->second == id) {
          it = _records_by_timestamp.erase(it);
        } else {
          it++;
        }
      }
    }
    {
      auto [start, stop] = _records_by_karma.equal_range(ref_to_record.karma);
      for (auto &it = start; it != stop;) {
        if (it->second == id) {
          it = _records_by_karma.erase(it);
        } else {
          it++;
        }
      }
    }

    {
      auto [start, stop] = _records_by_user.equal_range(ref_to_record.user);
      for (auto &it = start; it != stop;) {
        if (it->second == id) {
          it = _records_by_user.erase(it);
        } else {
          it++;
        }
      }
    }
    _records.erase(it);
    return true;
  }

  template <typename Callback>
  void RangeByTimestamp(int low, int high, Callback callback) const {
    auto start = _records_by_timestamp.lower_bound(low);
    auto end = _records_by_timestamp.upper_bound(high);
    bool stop = false;
    for (auto &it = start; it != end && !stop; it++) {
      stop = !callback(*GetById(string(it->second)));
    }
  }

  template <typename Callback>
  void RangeByKarma(int low, int high, Callback callback) const {
    // for (auto it = _records_by_karma.begin(); it != _records_by_karma.end();
    //      it++) {
    //   cout << it->first << " / " << it->second << endl;
    // }
    auto start = _records_by_karma.lower_bound(low);
    auto end = _records_by_karma.upper_bound(high);
    bool stop = false;
    for (auto &it = start; it != end && !stop; it++) {
      stop = !callback(*GetById(string(it->second)));
    }
  }

  template <typename Callback>
  void AllByUser(const string &user, Callback callback) const {
    // for (auto it = _records_by_user.begin(); it != _records_by_user.end();
    //      it++) {
    //   cout << it->first << " / " << it->second << endl;
    // }
    auto start = _records_by_user.lower_bound(user);
    auto end = _records_by_user.upper_bound(user);
    bool stop = false;
    for (auto &it = start; it != end && !stop; it++) {
      stop = !callback(*GetById(string(it->second)));
    }
  }

private:
  map<string, Record> _records;
  multimap<int, string_view> _records_by_timestamp;
  multimap<int, string_view> _records_by_karma;
  multimap<string_view, string_view> _records_by_user;
};

void TestRangeBoundaries() {
  const int good_karma = 1000;
  const int bad_karma = -10;

  Database db;
  db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
  db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

  int count = 0;
  db.RangeByKarma(bad_karma, good_karma, [&count](const Record &) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestSameUser() {
  Database db;
  db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
  db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

  {
    int count = 0;
    db.AllByUser("master", [&count](const Record &) {
      ++count;
      return true;
    });

    ASSERT_EQUAL(2, count);
  }

  db.Erase("id2");
  {
    int count = 0;
    db.AllByUser("master", [&count](const Record &) {
      ++count;
      return true;
    });

    ASSERT_EQUAL(1, count);
  }

  db.Put({"id22", "Rethink life", "master", 1536107260, 2000});
  db.Put({"id232", "xx life", "master", 1536107260, 2});
  {
    int count = 0;
    db.AllByUser("master", [&count](const Record &) {
      ++count;
      return true;
    });

    ASSERT_EQUAL(3, count);
  }
}

void TestReplacement() {
  const string final_body = "Feeling sad";

  Database db;
  db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
  db.Erase("id");
  db.Put({"id", final_body, "not-master", 1536107260, -10});

  auto record = db.GetById("id");
  ASSERT(record != nullptr);
  ASSERT_EQUAL(final_body, record->title);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestRangeBoundaries);
  RUN_TEST(tr, TestSameUser);
  RUN_TEST(tr, TestReplacement);
  return 0;
}
