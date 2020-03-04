#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

// TODO need custom hasher and comparator
// struct Domain {
//   string name;
//   unordered_set<shared_ptr<Domain>> children;
// };

struct IDomainContainer {
  virtual void Add(const string &domain) = 0;
  virtual bool Contains(string_view domain) const = 0;
};

class SortedDomainContainer : public IDomainContainer {
public:
  virtual void Add(const string &domain) override;
  void Repack();
  virtual bool Contains(string_view domain) const override;

private:
  set<string> _domains;
  vector<string> _domains_cache;
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

void SortedDomainContainer::Add(const string &domain) {
  _domains.insert(ReverseDomains(domain));
}

void SortedDomainContainer::Repack() {
  _domains_cache.assign(_domains.begin(), _domains.end());
}

bool comp(const string &a, const string &b) {
  if (a.length() > b.length()) {
    return b != a.substr(b.length());
  } else if (b.length() > a.length()) {
    return a != b.substr(a.length());
  } else {
    return a != b;
  }
}

bool SortedDomainContainer::Contains(string_view domain) const {
  size_t next_dot = 0;
  string next_sub_domain;
  set<string>::iterator start;
  set<string>::iterator finish;
  do {
    next_dot = domain.find('.');
    if (next_dot == string::npos) {
      next_sub_domain = domain;
    } else {
      next_sub_domain = domain.substr(0, next_dot));
      domain.remove_prefix(next_dot + 1);
    }
    tie(start, finish) = equal_range(
        _domains_cache.begin(), _domains_cache.end(), next_sub_domain, comp);
    if (start == _domains_cache.end()) {
      // not found
    } else {
      // found!
    }
  } while (next_dot != string::npos);
  return false;
}

void TestRevertDomain() {
  ASSERT_EQUAL(ReverseDomains("com"), "com");
  ASSERT_EQUAL(ReverseDomains("yandex.ru"), "ru.yandex");
  ASSERT_EQUAL(ReverseDomains("mail.yandex.ru"), "ru.yandex.mail");
}

void RunAllTests() {
  TestRunner tr;
  RUN_TEST(tr, TestRevertDomain);
}

int main() { RunAllTests(); }
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

// vector<string> ReadDomains() {
//   size_t count;
//   cin >> count;

//   vector<string> domains;
//   for (size_t i = 0; i < count; ++i) {
//     string domain;
//     getline(cin, domain);
//     domains.push_back(domain);
//   }
//   return domains;
// }

// int main() {
//   const vector<string> banned_domains = ReadDomains();
//   const vector<string> domains_to_check = ReadDomains();

//   for (string_view domain : banned_domains) {
//     reverse(begin(domain), end(domain));
//   }
//   sort(begin(banned_domains), end(banned_domains));

//   size_t insert_pos = 0;
//   for (string &domain : banned_domains) {
//     if (insert_pos == 0 ||
//         !IsSubdomain(domain, banned_domains[insert_pos - 1])) {
//       swap(banned_domains[insert_pos++], domain);
//     }
//   }
//   banned_domains.resize(insert_pos);

//   for (const string_view domain : domains_to_check) {
//     if (const auto it =
//             upper_bound(begin(banned_domains), end(banned_domains), domain);
//         it != begin(banned_domains) && IsSubdomain(domain, *prev(it))) {
//       cout << "Good" << endl;
//     } else {
//       cout << "Bad" << endl;
//     }
//   }
//   return 0;
// }
