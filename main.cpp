#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

using namespace std;
bool LessOrEqualByChunks(const string &a, const string &b);
bool LessByChunks(const string &a, const string &b);
struct ChunksComparer {
  bool operator()(const string &a, const string &b) const {
    return LessByChunks(a, b);
  }
};

struct IDomainContainer {
  virtual void Add(const string &url) = 0;
  virtual void AddMany(const vector<string> &urs) = 0;
  virtual bool Contains(string url) const = 0;
  virtual vector<string> GetDomains() const = 0;
};

class SortedDomainContainer : public IDomainContainer {
public:
  virtual void Add(const string &url) override;
  virtual void AddMany(const vector<string> &urls) override;
  virtual bool Contains(string url) const override;
  vector<string> GetDomains() const override;

private:
  void Repack();
  set<string, ChunksComparer> _storage;
  vector<string> _urls;
};

string ReverseDomains(string_view domain) {
  string res;
  size_t next_dot;
  do {
    next_dot = domain.find('.');
    if (next_dot == string::npos) {
      res = string(domain) + res;
      break;
    } else {
      res = '.' + string(domain.substr(0, next_dot)) + res;
      domain.remove_prefix(next_dot + 1);
    }
  } while (true);
  return res;
}

vector<string> SortedDomainContainer::GetDomains() const {
  return {_urls.begin(), _urls.end()};
}

// Is a subdomain of b
bool IsSubDomain(const string &a, const string &b) {
  size_t next_a = -1, next_b = -1;

  do {
    next_a = a.find('.', next_a + 1);
    next_b = b.find('.', next_b + 1);

    if (a.substr(0, next_a) != b.substr(0, next_b)) {
      return false;
    }

    if (next_a == string::npos || next_b == string::npos) {
      return next_b == string::npos;
    }
  } while (true);
}

void SortedDomainContainer::Add(const string &url) {
  _storage.insert(ReverseDomains(url));
  Repack();
}

void SortedDomainContainer::AddMany(const vector<string> &urls) {
  for (const auto &url : urls) {
    _storage.insert(ReverseDomains(url));
  }
  Repack();
}

void SortedDomainContainer::Repack() {
  _urls.assign(_storage.begin(), _storage.end());
  // _urls.clear();
  // if (_storage.size() <= 1) {
  //   _urls.assign(_storage.begin(), _storage.end());
  // } else {
  //   _urls.reserve(_storage.size());
  //   size_t i = 0;
  //   _urls[0] = *(_storage.begin());
  //   for (auto it = next(_storage.begin()); it != _storage.end(); it++, i++) {
  // TODO here i can compare next with previous and filter out unnecessary stuff
  //   }
  //   _urls.shrink_to_fit();
  // }
}

template <class Iterator> struct Range {
  Iterator start;
  Iterator finish;
};

void FindEqualRange(Range<vector<string>::const_iterator> &range,
                    const string &value) {
  auto [start, finish] =
      equal_range(range.start, range.finish, value, LessByChunks);
  range = {start, finish};
}

void FindEqualRange2(Range<vector<string>::const_iterator> &range,
                     const string &value) {
  range = {lower_bound(range.start, range.finish, value, LessByChunks),
           lower_bound(range.start, range.finish, value, LessOrEqualByChunks)};
}

bool LessByChunks(const string &a, const string &b) {
  /*
  For all a, comp(a,a)==false
  If comp(a,b)==true then comp(b,a)==false
  if comp(a,b)==true and comp(b,c)==true then comp(a,c)==true
  */

  size_t next_a = -1, next_b = -1;

  do {
    next_a = a.find('.', next_a + 1);
    next_b = b.find('.', next_b + 1);

    if (a.substr(0, next_a) != b.substr(0, next_b)) {
      return a.substr(0, next_a) < b.substr(0, next_b);
    }

    if (next_a == string::npos && next_b == string::npos) {
      return false; // they are equal, hence not less
    } else if (next_a == string::npos) {
      return true; // a is shorter than b hence it is less
    } else if (next_b == string::npos) {
      return false; // a is longer than b hence it is greater, not less
    }
  } while (true);
}

bool LessOrEqualByChunks(const string &a, const string &b) {
  size_t next_a = -1, next_b = -1;

  do {
    next_a = a.find('.', next_a + 1);
    next_b = b.find('.', next_b + 1);

    if (a.substr(0, next_a) <= b.substr(0, next_b)) {
      return true;
    }

    if (next_a == string::npos && next_b == string::npos) {
      return false; // a is not less or equal than b, hence a > b
    } else if (next_a == string::npos) {
      return true; // a is shorter than b hence it is less
    } else if (next_b == string::npos) {
      return false; // a is longer than b hence it is greater, not less
    }
  } while (true);
}

bool SortedDomainContainer::Contains(string domain) const {
  size_t next_dot = -1;
  string next_sub_domain;

  Range<vector<string>::const_iterator> items = {_urls.begin(), _urls.end()};
  string reversed = ReverseDomains(domain);
  do {
    next_dot = reversed.find('.', next_dot + 1);
    if (next_dot == string::npos) {
      next_sub_domain = reversed;
    } else {
      next_sub_domain = reversed.substr(0, next_dot);
    }

    FindEqualRange2(items, next_sub_domain);
    for (auto it = items.start; it != items.finish; it++) {
      if (IsSubDomain(next_sub_domain, *it)) {
        return true;
      }
    }
  } while (next_dot != string::npos);
  return false;
}

void TestRevertDomain() {
  ASSERT_EQUAL(ReverseDomains("com"), "com");
  ASSERT_EQUAL(ReverseDomains("yandex.ru"), "ru.yandex");
  ASSERT_EQUAL(ReverseDomains("mail.yandex.ru"), "ru.yandex.mail");
}

void TestPrefixSort() {
  {
    vector<string> domains{"com", "ru"};
    vector<string> expected{"com", "ru"};

    sort(domains.begin(), domains.end(), LessByChunks);
    ASSERT_EQUAL(domains, expected);
  }
  {
    vector<string> domains{"com",     "ru",           "au.list",
                           "au.pain", "zh.fun.store", "zh.games"};
    vector<string> expected{"au.list", "au.pain",      "com",
                            "ru",      "zh.fun.store", "zh.games"};

    sort(domains.begin(), domains.end(), LessByChunks);
    ASSERT_EQUAL(domains, expected);
  }
  {
    vector<string> domains{
        "au",          "ru",          "au.list",     "au.pain",
        "au.list.wow", "au.lint.wow", "au.list.now",
    };
    vector<string> expected{
        "au",          "au.lint.wow", "au.list", "au.list.now",
        "au.list.wow", "au.pain",     "ru",
    };

    sort(domains.begin(), domains.end(), LessByChunks);
    ASSERT_EQUAL(domains, expected);
  }
}

void TestLessByChunksIsValidComp() {
  // For all a, comp(a,a)==false

  ASSERT(!LessByChunks("au", "au"));

  // If comp(a,b)==true then comp(b,a)==false
  ASSERT(LessByChunks("au", "au.love"));
  ASSERT(!LessByChunks("au.love", "au"));

  // if comp(a,b)==true and comp(b,c)==true then comp(a,c)==true
  ASSERT(LessByChunks("au.love", "au.mail"));
  ASSERT(LessByChunks("au.mail", "au.mailo"));
  ASSERT(LessByChunks("au.love", "au.mailo"));

  ///
  // FindEqualRange(range, "au.list.hoho");
  // ASSERT_EQUAL(*range.start, "au.list");
  // ASSERT_EQUAL(*range.finish, "au.pain");
  ASSERT(LessByChunks("au.list", "au.list.hoho"));
  ASSERT(LessByChunks("au.list.hoho", "au.pain"));
}

void TestIsSubDomain() {
  ASSERT(IsSubDomain("au.less", "au"));
  ASSERT(IsSubDomain("au", "au"));
  ASSERT(!IsSubDomain("au", "au.less"));
  ASSERT(!IsSubDomain("au.love", "au.less"));
  ASSERT(IsSubDomain("au.love.fun", "au.love"));
  ASSERT(!IsSubDomain("au.love", "au.love.fun"));
  ASSERT(!IsSubDomain("zh", "au"));
  ASSERT(!IsSubDomain("zh.love", "au.love"));
  ASSERT(!IsSubDomain("zh.love.you", "au.love"));
  ASSERT(!IsSubDomain("zh.love", "au.love.you"));
}

void TestAddSomeDomains() {
  SortedDomainContainer sdc;
  sdc.Add("ru");
  sdc.Add("zh");
  sdc.Add("oracle.com");
  sdc.Add("yandex.ru");
  vector<string> expected{"com.oracle", "ru", "ru.yandex", "zh"};
  ASSERT_EQUAL(sdc.GetDomains(), expected);
}

void TestFindEqualDomains() {
  SortedDomainContainer sdc;
  vector<string> input = {"com",     "ru",           "list.au",
                          "pain.au", "store.fun.zh", "games.zh"};
  sdc.AddMany(input);
  vector<string> expected{"au.list", "au.pain",      "com",
                          "ru",      "zh.fun.store", "zh.games"};
  ASSERT_EQUAL(sdc.GetDomains(), expected);
  {
    Range<vector<string>::const_iterator> range = {expected.cbegin(),
                                                   expected.cend()};
    FindEqualRange(range, "au");
    ASSERT_EQUAL(*range.start, "au.list");
    ASSERT_EQUAL(*range.finish, "au.list");
  }
  {
    Range<vector<string>::const_iterator> range = {expected.cbegin(),
                                                   expected.cend()};
    FindEqualRange(range, "au.list");
    ASSERT_EQUAL(*range.start, "au.list");
    ASSERT_EQUAL(*range.finish, "au.pain");
  }
  {
    Range<vector<string>::const_iterator> range = {expected.cbegin(),
                                                   expected.cend()};
    FindEqualRange(range, "au.list.hoho");
    // bad, does not show the previous one. But it's gonna be found on prev
    // step
    ASSERT_EQUAL(*range.start, "au.pain");
    ASSERT_EQUAL(*range.finish, "au.pain");
  }
  {
    Range<vector<string>::const_iterator> range = {expected.cbegin(),
                                                   expected.cend()};
    FindEqualRange(range, "au.pain");
    ASSERT_EQUAL(*range.start, "au.pain");
    ASSERT_EQUAL(*range.finish, "com");
  }
  {
    // searching for fun.fun.zh
    Range<vector<string>::const_iterator> range = {expected.cbegin(),
                                                   expected.cend()};
    FindEqualRange(range, "zh");
    ASSERT_EQUAL(*range.start, "zh.fun.store");
    ASSERT_EQUAL(*range.finish, "zh.fun.store");
    // iterators are equal => either not found or exact match
    FindEqualRange(range, "zh.fun");
    ASSERT_EQUAL(*range.start, "zh.fun.store");
    ASSERT_EQUAL(*range.finish, "zh.fun.store");
    // iterators are equal => either not found or exact match
    FindEqualRange(range, "zh.fun.fun");
    ASSERT_EQUAL(*range.start, "zh.fun.store");
    ASSERT_EQUAL(*range.finish, "zh.fun.store");
    // => not found
  }
  {
    // vector<string> expected{"au.list", "au.pain",      "com",
    //                         "ru",      "zh.fun.store", "zh.games"};
    // searching for fun.fun.zh
    Range<vector<string>::const_iterator> range = {expected.cbegin(),
                                                   expected.cend()};
    FindEqualRange2(range, "zh");
    ASSERT_EQUAL(*range.start, "zh.fun.store");
    ASSERT(range.finish == expected.end());
    // iterators are equal => either not found or exact match
    FindEqualRange2(range, "zh.fun");
    ASSERT_EQUAL(*range.start, "zh.fun.store");
    ASSERT(range.finish == expected.end());
    // iterators are equal => either not found or exact match
    FindEqualRange2(range, "zh.fun.fun");
    ASSERT_EQUAL(*range.start, "zh.fun.store");
    ASSERT(range.finish == expected.end());
    // => not found
  }

  {
    // vector<string> expected{"au.list", "au.pain",      "com",
    //                         "ru",      "zh.fun.store", "zh.games"};
    Range<vector<string>::const_iterator> range = {expected.cbegin(),
                                                   expected.cend()};
    FindEqualRange2(range, "au");
    ASSERT_EQUAL(*range.start, "au.list");
    ASSERT_EQUAL(*range.finish, "com");

    FindEqualRange2(range, "au.pain");
    ASSERT_EQUAL(*range.start, "au.pain");
    ASSERT_EQUAL(*range.finish, "com");

    FindEqualRange2(range, "au.pain.mail");
    ASSERT_EQUAL(*range.start, "com");
    ASSERT_EQUAL(*range.finish, "com");
  }
  {
    // vector<string> expected{"au.list", "au.pain",      "com",
    //                         "ru",      "zh.fun.store", "zh.games"};
    Range<vector<string>::const_iterator> range = {expected.cbegin(),
                                                   expected.cend()};
    FindEqualRange2(range, "au");
    ASSERT_EQUAL(*range.start, "au.list");
    ASSERT_EQUAL(*range.finish, "com");

    FindEqualRange2(range, "au.love");
    ASSERT_EQUAL(*range.start, "au.pain");
    ASSERT_EQUAL(*range.finish, "com");
  }
  {
    // vector<string> expected{"au.list", "au.pain",      "com",
    //                         "ru",      "zh.fun.store", "zh.games"};
    Range<vector<string>::const_iterator> range = {expected.cbegin(),
                                                   expected.cend()};
    FindEqualRange2(range, "com");
    ASSERT_EQUAL(*range.start, "com");
    ASSERT_EQUAL(*range.finish, "ru");

    FindEqualRange2(range, "com.russia");
    ASSERT_EQUAL(*range.start, "ru");
    ASSERT_EQUAL(*range.finish, "ru");
  }
}

void TestContains() {
  SortedDomainContainer sdc;
  vector<string> input = {"com",     "ru",           "list.au",
                          "pain.au", "store.fun.zh", "games.zh"};
  sdc.AddMany(input);

  // Simple tests
  ASSERT(sdc.Contains("com"));
  ASSERT(sdc.Contains("ru"));
  ASSERT(sdc.Contains("list.au"));
  ASSERT(sdc.Contains("pain.au"));
  ASSERT(sdc.Contains("store.fun.zh"));
  ASSERT(sdc.Contains("games.zh"));

  // Second level tests
  ASSERT(sdc.Contains("foofle.com"));
  ASSERT(sdc.Contains("yandex.ru"));
  ASSERT(sdc.Contains("mail.list.au"));
  ASSERT(sdc.Contains("fun.pain.au"));
  ASSERT(sdc.Contains("wow.store.fun.zh"));
  ASSERT(sdc.Contains("xxx.games.zh"));

  ASSERT(!sdc.Contains("foofle.co"));
  ASSERT(!sdc.Contains("yandex.su"));
  ASSERT(!sdc.Contains("mail.lint.au"));
  ASSERT(!sdc.Contains("fun.pane.au"));
  ASSERT(!sdc.Contains("wow.whore.fun.zh"));
  ASSERT(!sdc.Contains("xxx.gamse.zh"));

  ASSERT(sdc.Contains("top.foofle.com"));
  ASSERT(sdc.Contains("a.ru"));
  ASSERT(!sdc.Contains("au"));
  ASSERT(!sdc.Contains("pan.au"));
  ASSERT(!sdc.Contains("zh"));
}

void RunAllTests() {
  TestRunner tr;
  RUN_TEST(tr, TestRevertDomain);
  RUN_TEST(tr, TestLessByChunksIsValidComp);
  RUN_TEST(tr, TestPrefixSort);
  RUN_TEST(tr, TestIsSubDomain);
  RUN_TEST(tr, TestAddSomeDomains);
  RUN_TEST(tr, TestFindEqualDomains);
  RUN_TEST(tr, TestContains);
}

// bool IsSubdomain(string_view subdomain, string_view domain) {
//   auto i = subdomain.size() - 1;
//   auto j = domain.size() - 1;
//   while (i >= 0 && j >= 0) {
//     if (subdomain[i--] != domain[j--]) {
//       return false;
//     }
//   }
//   return (i < 0 && domain[j] == '.') || (j < 0 && subdomain[i] == '.');
// }

vector<string> ReadDomains() {
  size_t count;
  cin >> count;

  vector<string> domains(count);
  string domain;
  for (size_t i = 0; i < count; ++i) {
    cin >> domain;
    domains[i] = domain;
  }
  return domains;
}

int main() {
  // RunAllTests();
  const vector<string> banned_domains = ReadDomains();
  const vector<string> domains_to_check = ReadDomains();

  SortedDomainContainer sdc;
  sdc.AddMany(banned_domains);

  for (const string &domain : domains_to_check) {
    if (sdc.Contains(domain)) {
      cout << "Bad " << endl;
    } else {
      cout << "Good" << endl;
    }
  }
  return 0;
}
