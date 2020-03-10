#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

template <typename It> class Range {
public:
  Range(It begin, It end) : begin_(begin), end_(end) {}
  It begin() const { return begin_; }
  It end() const { return end_; }

private:
  It begin_;
  It end_;
};

pair<string_view, optional<string_view>>
SplitTwoStrict(string_view s, string_view delimiter = " ") {
  const size_t pos = s.find(delimiter);
  if (pos == s.npos) {
    return {s, nullopt};
  } else {
    return {s.substr(0, pos), s.substr(pos + delimiter.length())};
  }
}

vector<string_view> Split(string_view s, string_view delimiter = " ") {
  vector<string_view> parts;
  if (s.empty()) {
    return parts;
  }
  while (true) {
    const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
    parts.push_back(lhs);
    if (!rhs_opt) {
      break;
    }
    s = *rhs_opt;
  }
  return parts;
}

class Domain {
public:
  explicit Domain(string_view text) {
    vector<string_view> parts = Split(text, ".");
    parts_reversed_.assign(rbegin(parts), rend(parts));
  }

  size_t GetPartCount() const { return parts_reversed_.size(); }

  auto GetParts() const {
    return Range(rbegin(parts_reversed_), rend(parts_reversed_));
  }
  auto GetReversedParts() const {
    return Range(begin(parts_reversed_), end(parts_reversed_));
  }

  bool operator==(const Domain &other) const {
    return parts_reversed_ == other.parts_reversed_;
  }

private:
  vector<string> parts_reversed_;
};

ostream &operator<<(ostream &stream, const Domain &domain) {
  bool first = true;
  for (const string_view part : domain.GetParts()) {
    if (!first) {
      stream << '.';
    } else {
      first = false;
    }
    stream << part;
  }
  return stream;
}

// domain is subdomain of itself
bool IsSubdomain(const Domain &subdomain, const Domain &domain) {
  const auto subdomain_reversed_parts = subdomain.GetReversedParts();
  const auto domain_reversed_parts = domain.GetReversedParts();
  return subdomain.GetPartCount() >= domain.GetPartCount() &&
         equal(begin(domain_reversed_parts), end(domain_reversed_parts),
               begin(subdomain_reversed_parts));
}

bool IsSubOrSuperDomain(const Domain &lhs, const Domain &rhs) {
  return lhs.GetPartCount() >= rhs.GetPartCount() ? IsSubdomain(lhs, rhs)
                                                  : IsSubdomain(rhs, lhs);
}

class DomainChecker {
public:
  template <typename InputIt>
  DomainChecker(InputIt domains_begin, InputIt domains_end) {
    sorted_domains_.reserve(distance(domains_begin, domains_end));
    for (const Domain &domain : Range(domains_begin, domains_end)) {
      sorted_domains_.push_back(&domain);
    }
    sort(begin(sorted_domains_), end(sorted_domains_), IsDomainLess);
    sorted_domains_ = AbsorbSubdomains(move(sorted_domains_));
  }

  // Check if candidate is subdomain of some domain
  bool IsSubdomain(const Domain &candidate) const {
    const auto it = upper_bound(begin(sorted_domains_), end(sorted_domains_),
                                &candidate, IsDomainLess);
    if (it == begin(sorted_domains_)) {
      return false;
    }
    return ::IsSubdomain(candidate, **prev(it));
  }

private:
  vector<const Domain *> sorted_domains_;

  static bool IsDomainLess(const Domain *lhs, const Domain *rhs) {
    const auto lhs_reversed_parts = lhs->GetReversedParts();
    const auto rhs_reversed_parts = rhs->GetReversedParts();
    return lexicographical_compare(
        begin(lhs_reversed_parts), end(lhs_reversed_parts),
        begin(rhs_reversed_parts), end(rhs_reversed_parts));
  }

  static vector<const Domain *>
  AbsorbSubdomains(vector<const Domain *> domains) {
    domains.erase(unique(begin(domains), end(domains),
                         [](const Domain *lhs, const Domain *rhs) {
                           return IsSubOrSuperDomain(*lhs, *rhs);
                         }),
                  end(domains));
    return domains;
  }
};

vector<Domain> ReadDomains(istream &in_stream = cin) {
  vector<Domain> domains;

  size_t count;
  in_stream >> count;
  domains.reserve(count);

  for (size_t i = 0; i < count; ++i) {
    string domain_text;
    in_stream >> domain_text;
    domains.emplace_back(domain_text);
  }
  return domains;
}

vector<bool> CheckDomains(const vector<Domain> &banned_domains,
                          const vector<Domain> &domains_to_check) {
  const DomainChecker checker(begin(banned_domains), end(banned_domains));

  vector<bool> check_results;
  check_results.reserve(domains_to_check.size());
  for (const Domain &domain_to_check : domains_to_check) {
    check_results.push_back(!checker.IsSubdomain(domain_to_check));
  }

  return check_results;
}

void PrintCheckResults(const vector<bool> &check_results,
                       ostream &out_stream = cout) {
  for (const bool check_result : check_results) {
    out_stream << (check_result ? "Good" : "Bad") << "\n";
  }
}

void TestSplitDoesNotAddEmpty() {
  ASSERT_EQUAL(Split("a b c").size(), 3);
  ASSERT_EQUAL(Split("").size(), 0);
  ASSERT_EQUAL(Split(" ").size(), 2);      // ?
  ASSERT_EQUAL(Split("  ").size(), 3);     // ?
  ASSERT_EQUAL(Split("a  b c").size(), 4); // ?
  ASSERT_EQUAL(Split(" a b c").size(), 4); // ?
}

void TestReverseParts() {
  {
    Domain d("ru");
    auto reversed = d.GetReversedParts();
    ASSERT_EQUAL(*reversed.begin(), "ru");
  }
  {
    Domain d("yandex.ru");
    auto reversed = d.GetReversedParts();
    ASSERT_EQUAL(*reversed.begin(), "ru");
    ASSERT_EQUAL(*next(reversed.begin()), "yandex");
  }
}

void TestSelfSubDomain() {
  {
    Domain d1("yandex.ru");
    Domain d2("yandex.ru");
    ASSERT(IsSubdomain(d1, d2));
  }
  {
    Domain d1("com");
    Domain d2("com");
    ASSERT(IsSubdomain(d1, d2));
  }
}

void TestSubDomain() {
  Domain d1("yandex.ru");
  Domain d2("ru");
  ASSERT(IsSubdomain(d1, d2));
  ASSERT(!IsSubdomain(d2, d1));
}

void TestDomainCheckerKillsSubdomains() {
  Domain d1("yandex.ru");
  Domain d2("ru");
  Domain d3("x.com");
  Domain d4("a.au");
  Domain d5("mail.ru");
  vector<Domain> x = {d1, d2, d3, d4, d5};
  DomainChecker dc(x.begin(), x.end());
  ASSERT(dc.IsSubdomain(Domain("ru")));
  ASSERT(dc.IsSubdomain(Domain("n.x.com")));
  ASSERT(!dc.IsSubdomain(Domain("n.y.com")));
  ASSERT(dc.IsSubdomain(Domain("yandex.ru")));
  ASSERT(dc.IsSubdomain(Domain("x.ru")));
  ASSERT(dc.IsSubdomain(Domain("web.yandex.ru")));
  ASSERT(dc.IsSubdomain(Domain("mail.web.yandex.ru")));
  ASSERT(dc.IsSubdomain(Domain("a.au")));
  ASSERT(!dc.IsSubdomain(Domain("au")));
}

void TestOneOfForbidden() {
  // no idea
}

void TestGoodVsBad() {
  Domain d1("yandex.ru");
  Domain d2("ru");
  Domain d3("x.com");
  Domain d4("a.au");
  Domain d5("mail.ru");
  vector<Domain> x = {d1, d2, d3, d4, d5};
  {
    ostringstream o;
    PrintCheckResults(CheckDomains({d1, d2, d3, d4, d5}, {Domain("some.ru")}),
                      o);
    ASSERT_EQUAL(o.str(), "Bad\n");
  }
  {
    ostringstream o;
    PrintCheckResults(
        CheckDomains({d1, d2, d3, d4, d5}, {Domain("abracadabra")}), o);
    ASSERT_EQUAL(o.str(), "Good\n");
  }
}

void TestFirstReadEmpty() {
  istringstream is("1\na\n");
  const vector<Domain> banned_domains = ReadDomains(is);
  ASSERT_EQUAL(banned_domains.size(), 1);
  ASSERT(banned_domains[0] == Domain("a"));
}

void RunAllTests() {
  TestRunner tr;
  RUN_TEST(tr, TestSplitDoesNotAddEmpty);
  RUN_TEST(tr, TestReverseParts);
  RUN_TEST(tr, TestSelfSubDomain);
  RUN_TEST(tr, TestSubDomain);
  RUN_TEST(tr, TestDomainCheckerKillsSubdomains);
  RUN_TEST(tr, TestOneOfForbidden);
  RUN_TEST(tr, TestGoodVsBad);
  RUN_TEST(tr, TestFirstReadEmpty);
}

int main() {
  RunAllTests();

  const vector<Domain> banned_domains = ReadDomains();
  const vector<Domain> domains_to_check = ReadDomains();
  PrintCheckResults(CheckDomains(banned_domains, domains_to_check));
  return 0;
}
