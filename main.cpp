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
      _records_by_timestamp[record.timestamp].insert(record_ref.id);
      _records_by_karma[record.karma].insert(record_ref.id);
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
    bool stop = false;
    for (auto &it = start; it != end && !stop; it++) {
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
  map<string, Record> _records;
  map<int, unordered_set<string_view>> _records_by_timestamp;
  map<int, unordered_set<string_view>> _records_by_karma;
  map<string_view, unordered_set<string_view>> _records_by_user;
};

int GetUserCount(Database db, string name) {
  int count = 0;
  db.AllByUser(name, [&count](const Record &) {
    ++count;
    return true;
  });
  return count;
}

int GetKarmaCount(Database db, int from, int to) {
  int count = 0;
  db.RangeByKarma(from, to, [&count](const Record &) {
    ++count;
    return true;
  });
  return count;
}
int GetTimeCount(Database db, int from, int to) {
  int count = 0;
  db.RangeByTimestamp(from, to, [&count](const Record &) {
    ++count;
    return true;
  });

  return count;
}

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
  db.Put({"id222", "Rethink life", "ss", 1536107260, 2000});
  db.Put({"id232", "xx life", "master", 1536107260, 2});
  {
    int count = 0;
    db.AllByUser("master", [&count](const Record &) {
      ++count;
      return true;
    });

    ASSERT_EQUAL(3, count);
  }
  {
    int count = 0;
    db.AllByUser("master", [&count](const Record &) {
      ++count;
      return false;
    });

    ASSERT_EQUAL(1, count);
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

void TestDoubleInsert() {
  Database db;
  ASSERT(db.Put({"id", "Have a hand", "not-master", 1536107260, 10}));
  ASSERT(!db.Put({"id", "Have a hand", "not-master", 1536107260, 10}));
  ASSERT(!db.Put({"id", "Have a hand 2", "not-master 2", 1536107262, 12}));
  ASSERT(db.Put({"id2", "Have a hand", "not-master", 1536107260, 10}));
  ASSERT(!db.Put({"id2", "Have a hand", "not-master", 1536107260, 10}));
  db.Erase("id");
  ASSERT(db.Put({"id", "Have a hand 2", "not-master 2", 1536107262, 12}));
  ASSERT(!db.Put({"id2", "Have a hand", "not-master", 1536107260, 10}));
};

void TestDoubleInsertSecondaryKey() {
  Database db;
  ASSERT(db.Put({"1", "A", "a", 10, 3}));
  ASSERT(db.Put({"2", "A", "a", 20, 3}));
  ASSERT(*(db.GetById("1")) == Record({"1", "A", "a", 10, 3}));
  ASSERT(*(db.GetById("2")) == Record({"2", "A", "a", 20, 3}));

  ASSERT_EQUAL(GetKarmaCount(db, 3, 3), 2);
  ASSERT_EQUAL(GetTimeCount(db, 10, 10), 1);
  ASSERT_EQUAL(GetTimeCount(db, 20, 20), 1);
  ASSERT_EQUAL(GetTimeCount(db, 10, 20), 2);

  ASSERT(!db.Erase("0"));
  ASSERT(db.Erase("1"));
  ASSERT(!db.Erase("1"));

  ASSERT_EQUAL(GetKarmaCount(db, 3, 3), 1);
  ASSERT_EQUAL(GetTimeCount(db, 10, 10), 0);
  ASSERT_EQUAL(GetTimeCount(db, 20, 20), 1);
  ASSERT_EQUAL(GetTimeCount(db, 10, 20), 1);

  ASSERT(!db.Put({"2", "A", "a", 20, 3}));

  ASSERT_EQUAL(GetKarmaCount(db, 3, 3), 1);
  ASSERT_EQUAL(GetTimeCount(db, 10, 10), 0);
  ASSERT_EQUAL(GetTimeCount(db, 20, 20), 1);
  ASSERT_EQUAL(GetTimeCount(db, 10, 20), 1);

  ASSERT(db.Put({"1", "A", "a", 10, 3}));

  ASSERT_EQUAL(GetKarmaCount(db, 3, 3), 2);
  ASSERT_EQUAL(GetTimeCount(db, 10, 10), 1);
  ASSERT_EQUAL(GetTimeCount(db, 20, 20), 1);
  ASSERT_EQUAL(GetTimeCount(db, 10, 20), 2);
}

void TestGetByIdAndErase() {
  Database db;
  ASSERT(db.Put({"id", "Have a hand", "not-master", 1536107260, 10}));
  ASSERT(db.GetById("id") != nullptr);
  ASSERT(db.Erase("id"));
  ASSERT(!db.Erase("id"));
  ASSERT(db.GetById("id") == nullptr);
  ASSERT(db.Put({"id", "Have a hand 2", "not-master 2", 1536107262, 12}));
  ASSERT(db.GetById("id") != nullptr);
  ASSERT(!db.Erase("id2"));
}

void TestTimeIntervals() {
  Database db;
  ASSERT(db.Put({"1", "A", "a", 1, 1}));
  ASSERT(db.Put({"2", "A", "a", 1, 1}));
  ASSERT(db.Put({"3", "B", "b", 2, 2}));
  ASSERT(db.Put({"4", "C", "c", 4, 4}));
  ASSERT(db.Put({"5", "C", "c", 4, 4}));
  ASSERT(db.Put({"6", "C", "c", 4, 4}));
  ASSERT(db.Put({"7", "D", "d", 5, 5}));
  ASSERT(db.Put({"8", "F", "f", 7, 7}));

  ASSERT_EQUAL(GetUserCount(db, "a"), 2);
  ASSERT_EQUAL(GetUserCount(db, "b"), 1);
  ASSERT_EQUAL(GetUserCount(db, "c"), 3);
  ASSERT_EQUAL(GetUserCount(db, "d"), 1);
  ASSERT_EQUAL(GetUserCount(db, "e"), 0);
  ASSERT_EQUAL(GetUserCount(db, "f"), 1);

  ASSERT_EQUAL(GetKarmaCount(db, 1, 1), 2);
  ASSERT_EQUAL(GetKarmaCount(db, 1, 2), 3);
  ASSERT_EQUAL(GetKarmaCount(db, 1, 3), 3);
  ASSERT_EQUAL(GetKarmaCount(db, 1, 4), 6);
  ASSERT_EQUAL(GetKarmaCount(db, 1, 5), 7);
  ASSERT_EQUAL(GetKarmaCount(db, 1, 7), 8);
  ASSERT_EQUAL(GetKarmaCount(db, 1, 8), 8);

  ASSERT_EQUAL(GetKarmaCount(db, 2, 1), 0);
  ASSERT_EQUAL(GetKarmaCount(db, 2, 2), 1);
  ASSERT_EQUAL(GetKarmaCount(db, 2, 4), 4);
  ASSERT_EQUAL(GetKarmaCount(db, 2, 5), 5);
  ASSERT_EQUAL(GetKarmaCount(db, 2, 6), 5);
  ASSERT_EQUAL(GetKarmaCount(db, 2, 7), 6);
  ASSERT_EQUAL(GetKarmaCount(db, 2, 8), 6);

  ASSERT_EQUAL(GetKarmaCount(db, 3, 1), 0);
  ASSERT_EQUAL(GetKarmaCount(db, 3, 2), 0);
  ASSERT_EQUAL(GetKarmaCount(db, 3, 4), 3);
  ASSERT_EQUAL(GetKarmaCount(db, 3, 5), 4);
  ASSERT_EQUAL(GetKarmaCount(db, 3, 6), 4);
  ASSERT_EQUAL(GetKarmaCount(db, 3, 7), 5);
  ASSERT_EQUAL(GetKarmaCount(db, 3, 8), 5);

  ASSERT_EQUAL(GetKarmaCount(db, 7, 8), 1);
  ASSERT_EQUAL(GetKarmaCount(db, 8, 8), 0);
  ASSERT_EQUAL(GetKarmaCount(db, 9, 8), 0);
  ASSERT_EQUAL(GetKarmaCount(db, 9, 9), 0);

  ASSERT_EQUAL(GetTimeCount(db, 1, 1), 2);
  ASSERT_EQUAL(GetTimeCount(db, 1, 2), 3);
  ASSERT_EQUAL(GetTimeCount(db, 1, 3), 3);
  ASSERT_EQUAL(GetTimeCount(db, 1, 4), 6);
  ASSERT_EQUAL(GetTimeCount(db, 1, 5), 7);
  ASSERT_EQUAL(GetTimeCount(db, 1, 7), 8);
  ASSERT_EQUAL(GetTimeCount(db, 1, 8), 8);

  ASSERT_EQUAL(GetTimeCount(db, 2, 1), 0);
  ASSERT_EQUAL(GetTimeCount(db, 2, 2), 1);
  ASSERT_EQUAL(GetTimeCount(db, 2, 4), 4);
  ASSERT_EQUAL(GetTimeCount(db, 2, 5), 5);
  ASSERT_EQUAL(GetTimeCount(db, 2, 6), 5);
  ASSERT_EQUAL(GetTimeCount(db, 2, 7), 6);
  ASSERT_EQUAL(GetTimeCount(db, 2, 8), 6);

  ASSERT_EQUAL(GetTimeCount(db, 3, 1), 0);
  ASSERT_EQUAL(GetTimeCount(db, 3, 2), 0);
  ASSERT_EQUAL(GetTimeCount(db, 3, 4), 3);
  ASSERT_EQUAL(GetTimeCount(db, 3, 5), 4);
  ASSERT_EQUAL(GetTimeCount(db, 3, 6), 4);
  ASSERT_EQUAL(GetTimeCount(db, 3, 7), 5);
  ASSERT_EQUAL(GetTimeCount(db, 3, 8), 5);

  ASSERT_EQUAL(GetTimeCount(db, 7, 8), 1);
  ASSERT_EQUAL(GetTimeCount(db, 8, 8), 0);
  ASSERT_EQUAL(GetTimeCount(db, 9, 8), 0);
  ASSERT_EQUAL(GetTimeCount(db, 9, 9), 0);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestRangeBoundaries);
  RUN_TEST(tr, TestSameUser);
  RUN_TEST(tr, TestRangeBoundaries);

  RUN_TEST(tr, TestDoubleInsert);
  RUN_TEST(tr, TestGetByIdAndErase);
  RUN_TEST(tr, TestTimeIntervals);
  RUN_TEST(tr, TestDoubleInsertSecondaryKey);
  return 0;
}
