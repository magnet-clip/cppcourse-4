#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

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