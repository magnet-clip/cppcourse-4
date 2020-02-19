#include "test.h"
#include "db.h"
#include "test_runner.h"

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

  ASSERT(db.GetById("0") == nullptr);
  ASSERT(!db.Erase("0"));
  ASSERT(db.GetById("0") == nullptr);
  ASSERT(*(db.GetById("1")) == Record({"1", "A", "a", 10, 3}));
  ASSERT(db.Erase("1"));
  ASSERT(db.GetById("1") == nullptr);
  ASSERT(!db.Erase("1"));
  ASSERT(db.GetById("1") == nullptr);

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
  ASSERT(!db.Put({"1", "X", "y", 11, 4}));
  ASSERT(*(db.GetById("1")) == Record({"1", "A", "a", 10, 3}));

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

  ASSERT_EQUAL(GetUserCount(db, ""), 0);
  ASSERT_EQUAL(GetUserCount(db, "a"), 2);
  ASSERT_EQUAL(GetUserCount(db, "b"), 1);
  ASSERT_EQUAL(GetUserCount(db, "c"), 3);
  ASSERT_EQUAL(GetUserCount(db, "d"), 1);
  ASSERT_EQUAL(GetUserCount(db, "e"), 0);
  ASSERT_EQUAL(GetUserCount(db, "f"), 1);
  ASSERT_EQUAL(GetUserCount(db, "asdsadasdsad"), 0);

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
  ASSERT_EQUAL(GetKarmaCount(db, -10000, 10000), 8);

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
  ASSERT_EQUAL(GetTimeCount(db, -10000, 10000), 8);
}