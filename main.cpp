#include "test_runner.h"

#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std;

// Months start from 1
struct Month {
  explicit Month(int month) : _month(month) {
    if (month <= 0) throw invalid_argument("month is <= 0");
    if (month > 12) throw invalid_argument("month is > 12");
  }
  operator int() const { return _month; }
  Month &operator=(const Month &other) {
    _month = other._month;
    return *this;
  }

  Month NextMonth() const {
    if (_month < 12) {
      return Month(_month + 1);
    } else {
      return Month(1);
    }
  }

 private:
  int _month;
};

// Days start from 1
struct Day {
  explicit Day(int day) : _day(day) {
    if (day <= 0) throw invalid_argument("day is <= 0");
  }
  operator int() const { return _day; }
  Day &operator=(const Day &other) {
    _day = other._day;
    return *this;
  }

 private:
  int _day;
};

struct Year {
  explicit Year(int year) : _year(year) {}
  Year(const Year &other) : _year(other._year) {}

  operator int() const { return _year; }

  Year &operator=(const Year &other) {
    _year = other._year;
    return *this;
  }

  bool IsLeap() const {
    if (_year % 400 == 0) {
      return true;
    }
    if (_year % 100 == 0) {
      return false;
    }
    if (_year % 4 == 0) {
      return true;
    }
    return false;
  }

  int DaysInYear() const { return IsLeap() ? 366 : 365; }

  int DaysInMonth(const Month &month) const {
    return MonthLengths[month - 1] + ((month == 2 && IsLeap()) ? 1 : 0);
  }

  Year &operator++() {
    _year++;
    return *this;
  }

  Year operator++(int) {
    Year temp = *this;
    _year++;
    return temp;
  }

  Year Next() { return Year(_year + 1); }

 private:
  static const array<int, 12> MonthLengths;
  int _year;
};

const array<int, 12> Year::MonthLengths = {31, 28, 31, 30, 31, 30,
                                           31, 31, 30, 31, 30, 31};
struct Date {
  Date() : year(Year(0)), month(Month(1)), day(Day(1)) {
    if (day > year.DaysInMonth(month)) {
      throw invalid_argument("day too large");
    }
  }

  Date(Year new_year, Month new_month, Day new_day)
      : year(new_year), month(new_month), day(new_day) {}

  Date(const Date &other)
      : year(other.year), month(other.month), day(other.day) {}

  Date &operator=(const Date &other) {
    year = other.year;
    month = other.month;
    day = other.day;
    return *this;
  }

  Date &operator++() {
    *this = Next();
    return *this;
  }

  Date operator++(int) {
    Date temp = *this;
    *this = Next();
    return temp;
  }

  void AddYears(int add_years) { year = Year(year + add_years); }

  void AddMonths(int add_months) {
    int add_years = add_months / 12;
    add_months = add_months % 12;
    if (month + add_months > 12) {
      AddYears(add_years + 1);
      month = Month((month + add_months) % 12);
    } else {
      AddYears(add_years);
      month = Month(month + add_months);
    }
  }

  void AddDays(int add_days) {
    // Adding years
    while (add_days >= year.DaysInYear()) {
      add_days -= year.DaysInYear();
      auto next_year = year.Next();
      if ((year.IsLeap() == next_year.IsLeap()) || (month <= 2)) {
        year = next_year;
      } else {
        auto last_year = year;
        year = next_year;
        if (last_year.IsLeap()) {
          *this = Next();
        } else {
          *this = Prev();
        }
      }
    }

    // Adding months and days
    auto days_till_end = year.DaysInMonth(month) - day;
    while (add_days > days_till_end) {
      day = Day(1);
      AddMonths(1);
      add_days -= (days_till_end + 1);
      days_till_end = year.DaysInMonth(month) - day;
    }

    day = Day(day + add_days);
  }

  Date Next() const {
    int new_day = day + 1;
    int new_month = month;
    int new_year = year;
    if (new_day > year.DaysInMonth(month)) {
      // i will not consider situation when day was 45 and that
      // should have been treated as 15th of next month. yet.
      new_day = 1;
      new_month += 1;
      if (new_month > 12) {
        new_month = 1;
        new_year += 1;
      }
    }
    return Date(Year(new_year), Month(new_month), Day(new_day));
  }

  Date Prev() const {
    int new_day = day - 1;
    int new_month = month;
    int new_year = year;
    if (new_day < 1) {
      // i will not consider situation when day was 45 and that
      // should have been treated as 15th of next month. yet.
      new_month -= 1;
      if (new_month < 1) {
        new_month = 12;
        new_year -= 1;
      }
      new_day = year.DaysInMonth(Month(new_month));
    }
    return Date(Year(new_year), Month(new_month), Day(new_day));
  }

  Year year;
  Month month;
  Day day;
};

istream &operator>>(istream &is, Date &date) {
  int year = 0, month = 0, day = 0;
  is >> year;
  is.ignore(1);
  is >> month;
  is.ignore(1);
  is >> day;
  date = Date(Year(year), Month(month), Day(day));
  return is;
}

bool operator==(const Date &d1, const Date &d2) {
  return make_tuple(d1.year, d1.month, d1.day) ==
         make_tuple(d2.year, d2.month, d2.day);
}

bool operator!=(const Date &d1, const Date &d2) {
  return make_tuple(d1.year, d1.month, d1.day) !=
         make_tuple(d2.year, d2.month, d2.day);
}

bool operator<(const Date &d1, const Date &d2) {
  return make_tuple(d1.year, d1.month, d1.day) <
         make_tuple(d2.year, d2.month, d2.day);
}

bool operator>(const Date &d1, const Date &d2) {
  return make_tuple(d1.year, d1.month, d1.day) >
         make_tuple(d2.year, d2.month, d2.day);
}

ostream &operator<<(ostream &os, const Date &date) {
  return os << setfill('0') << setw(4) << date.year << '-' << setw(2)
            << date.month << '-' << setw(2) << date.day;
}

// d1 < d2
int YearlyDiff(const Date &d1, const Date &d2) {
  int year_days = 0;
  for (Year year = d1.year; year != d2.year; year++) {
    year_days += year.DaysInYear();
    auto next_year = year.Next();
    // TODO: this is same logic as in Date::AddDays, but AddDays is less
    // structured. Could it be taken out?
    if ((year.IsLeap() == next_year.IsLeap()) || (d1.month <= 2)) {
    } else {
      if (year.IsLeap()) {
        year_days -= 1;
      } else {
        year_days += 1;
      }
    }
  }
  return year_days;
}

// d1 < d2
int MonthlyDiff(const Year year, const Date &d1, const Date &d2) {
  int month_days = 0;
  for (Month m = d1.month; m != d2.month; m = m.NextMonth()) {
    month_days += year.DaysInMonth(m);
  }
  return month_days;
}

// d1 < d2
int DailyDiff(const Date &d1, const Date &d2) { return d2.day - d1.day; }

// d1 is expected to be greater than d2
int operator-(Date d1, Date d2) {
  int res = 0;

  if (d1 < d2) {
    res -= YearlyDiff(d1, d2);
    d1.year = d2.year;
  } else {
    res += YearlyDiff(d2, d1);
    d2.year = d1.year;
  }

  auto year = d1.year;

  if (d1 < d2) {
    res -= MonthlyDiff(year, d1, d2);
    d1.month = d2.month;
  } else {
    res += MonthlyDiff(year, d2, d1);
    d2.month = d1.month;
  }

  if (d1 < d2) {
    res -= DailyDiff(d1, d2);
  } else {
    res += DailyDiff(d2, d1);
  }

  return res;
}

struct Commands {
  static string ComputeIncomeCommand;
  static string EarnCommand;
  static string PayTaxCommand;
};

string Commands::ComputeIncomeCommand = "ComputeIncome";
string Commands::EarnCommand = "Earn";
string Commands::PayTaxCommand = "PayTax";
struct Command {
  Command(Date new_from, Date new_to) : from(new_from), to(new_to) {}
  Date from;
  Date to;
  virtual string Kind() const = 0;
  virtual string ToString() const = 0;
  virtual bool operator==(const Command &other) const = 0;
};

struct SimpleCommand : public Command {
  using Command::Command;
  virtual string ToString() const override {
    ostringstream os;
    cout << from;
    os << Kind() << ": from " << from << "; to: " << to << endl;
    return os.str();
  }
  virtual bool operator==(const Command &other) const override {
    return make_tuple(Kind(), from, to) ==
           make_tuple(other.Kind(), other.from, other.to);
  }
};

struct ComputeIncomeCommand : public SimpleCommand {
  using SimpleCommand::SimpleCommand;
  virtual string Kind() const override {
    return Commands::ComputeIncomeCommand;
  }
};

struct PayTaxCommand : public SimpleCommand {
  using SimpleCommand::SimpleCommand;
  virtual string Kind() const override { return Commands::PayTaxCommand; }
};

struct EarnCommand : public Command {
  EarnCommand(Date from, Date to, int new_value)
      : Command(from, to), value(new_value) {}
  int value;
  virtual string Kind() const override { return Commands::EarnCommand; }
  virtual string ToString() const override {
    ostringstream os;
    os << Kind() << ": from " << from << "; to: " << to << "; value: " << value
       << endl;
    return os.str();
  }
  virtual bool operator==(const Command &other) const override {
    if (make_tuple(Kind(), from, to) !=
        make_tuple(other.Kind(), other.from, other.to)) {
      return false;
    } else {
      auto earn_other = static_cast<const EarnCommand &>(other);
      return value == earn_other.value;
    }
  }
};

ostream &operator<<(ostream &os, const Command &command) {
  return os << command.ToString();
}

vector<shared_ptr<Command>> ReadCommands(istream &is = cin) {
  int request_count = 0;
  is >> request_count;
  vector<shared_ptr<Command>> res;
  for (int i = 0; i < request_count; i++) {
    string command;
    is >> command;

    Date from, to;
    is >> from >> to;

    if (command == Commands::ComputeIncomeCommand) {
      res.push_back(make_shared<ComputeIncomeCommand>(from, to));
    } else if (command == Commands::EarnCommand) {
      int value;
      is >> value;
      res.push_back(make_shared<EarnCommand>(from, to, value));
    } else if (command == Commands::PayTaxCommand) {
      res.push_back(make_shared<PayTaxCommand>(from, to));
    }
  }
  return res;
}

class DateRangeIterator {
 public:
  explicit DateRangeIterator(Date date) : _date(date) {}
  DateRangeIterator &operator++() {
    _date++;
    return *this;
  }
  Date &operator*() { return _date; }

 private:
  Date _date;
};

bool operator==(DateRangeIterator &d1, DateRangeIterator &d2) {
  return *d1 == *d2;
}

bool operator!=(DateRangeIterator &d1, DateRangeIterator &d2) {
  return *d1 != *d2;
}

class DateRange {
 public:
  DateRange(Date from, Date to) : _from(from), _to(to) {}
  int Days() const { return _to - _from; }
  DateRangeIterator begin() const { return DateRangeIterator(_from); }
  DateRangeIterator end() const { return DateRangeIterator(_to.Next()); }

 private:
  Date _from, _to;
};

class CommandProcessor {
 public:
  void Earn(const EarnCommand &earn) {
    auto range = DateRange(earn.from, earn.to);
    double days = range.Days() + 1;
    for (const auto &d : range) {
      _earnings[d] += earn.value / days;
    }
  }
  string ComputeIncome(const ComputeIncomeCommand &compute) {
    auto range = DateRange(compute.from, compute.to);
    double days = range.Days() + 1;
    double sum = 0;
    for (const auto &d : range) {
      sum += _earnings[d];
    }
    return to_string(sum);
  }
  void PayTax(const PayTaxCommand &pay) {
    auto range = DateRange(pay.from, pay.to);
    double days = range.Days() + 1;
    for (const auto &d : range) {
      _earnings[d] *= 0.87;
    }
  }

  const map<Date, double> &GetEarnings() const { return _earnings; }

 private:
  map<Date, double> _earnings;
};

vector<string> ProcessCommands(vector<shared_ptr<Command>> commands) {
  CommandProcessor processor;
  vector<string> answers;
  for (auto &command : commands) {
    if (command->Kind() == Commands::ComputeIncomeCommand) {
      auto compute = static_cast<ComputeIncomeCommand &>(*command);
      auto amount = processor.ComputeIncome(compute);
      answers.push_back(amount);
    } else if (command->Kind() == Commands::EarnCommand) {
      auto earn = static_cast<EarnCommand &>(*command);
      processor.Earn(earn);
    } else if (command->Kind() == Commands::PayTaxCommand) {
      auto pay = static_cast<PayTaxCommand &>(*command);
      processor.PayTax(pay);
    }
  }
  return answers;
}

void TestDates() {
  {
    stringstream s;
    s << "0000-01-01";
    Date d;
    s >> d;
    ASSERT_EQUAL(d, Date());
  }
  {
    stringstream s;
    s << "1991-01-02";
    Date d;
    s >> d;
    ASSERT_EQUAL(d, Date(Year(1991), Month(1), Day(2)));
  }
  {
    stringstream s;
    s << "3-01-02";
    Date d;
    s >> d;
    ASSERT_EQUAL(d, Date(Year(3), Month(1), Day(2)));
  }
}

void TestDateInc() {
  {
    Date d;
    d++;
    ASSERT_EQUAL(d, Date(Year(0), Month(1), Day(2)));
  }
  {
    Date d(Year(2000), Month(1), Day(31));
    d++;
    ASSERT_EQUAL(d, Date(Year(2000), Month(2), Day(1)));
  }
  {
    Date d(Year(2000), Month(12), Day(31));
    d++;
    ASSERT_EQUAL(d, Date(Year(2001), Month(1), Day(1)));
  }
  {
    Date d(Year(2000), Month(2), Day(28));
    d++;
    ASSERT_EQUAL(d, Date(Year(2000), Month(2), Day(29)));
  }
  {
    Date d(Year(2100), Month(2), Day(28));
    d++;
    ASSERT_EQUAL(d, Date(Year(2100), Month(3), Day(1)));
  }
  {
    Date d(Year(2001), Month(2), Day(28));
    d++;
    ASSERT_EQUAL(d, Date(Year(2001), Month(3), Day(1)));
  }
  {
    Date d(Year(2004), Month(2), Day(28));
    d++;
    ASSERT_EQUAL(d, Date(Year(2004), Month(2), Day(29)));
  }
  {
    // Test for a case when day is out of months bounds. If i change it, this
    // test will fail
    Date d(Year(2011), Month(4), Day(100));
    d++;
    ASSERT_EQUAL(d, Date(Year(2011), Month(5), Day(1)));
  }
}

void TestDateAdditionDays() {
  {
    Date d(Year(1998), Month(5), Day(1));
    d.AddDays(31);
    ASSERT_EQUAL(d, Date(Year(1998), Month(6), Day(1)))
  }
  {
    Date d(Year(1998), Month(5), Day(31));
    d.AddDays(31);
    ASSERT_EQUAL(d, Date(Year(1998), Month(7), Day(1)))
  }
  {
    Date d(Year(1998), Month(6), Day(1));
    d.AddDays(30);
    ASSERT_EQUAL(d, Date(Year(1998), Month(7), Day(1)))
  }
  {
    Date d(Year(1998), Month(6), Day(30));
    d.AddDays(30);
    ASSERT_EQUAL(d, Date(Year(1998), Month(7), Day(30)))
  }
  // 1998 -> 1999 ; No leap -> No leap
  {
    Date d(Year(1998), Month(1), Day(1));
    d.AddDays(365);
    ASSERT_EQUAL(d, Date(Year(1999), Month(1), Day(1)))
  }
  {
    Date d(Year(1998), Month(1), Day(1));
    d.AddDays(366);
    ASSERT_EQUAL(d, Date(Year(1999), Month(1), Day(2)))
  }
  {
    Date d(Year(1998), Month(2), Day(1));
    d.AddDays(365);
    ASSERT_EQUAL(d, Date(Year(1999), Month(2), Day(1)))
  }
  {
    Date d(Year(1998), Month(2), Day(1));
    d.AddDays(366);
    ASSERT_EQUAL(d, Date(Year(1999), Month(2), Day(2)))
  }
  {
    Date d(Year(1998), Month(3), Day(1));
    d.AddDays(365);
    ASSERT_EQUAL(d, Date(Year(1999), Month(3), Day(1)))
  }
  {
    Date d(Year(1998), Month(2), Day(1));
    d.AddDays(366);
    ASSERT_EQUAL(d, Date(Year(1999), Month(2), Day(2)))
  }
  {
    Date d(Year(1998), Month(4), Day(1));
    d.AddDays(365);
    ASSERT_EQUAL(d, Date(Year(1999), Month(4), Day(1)))
  }
  {
    Date d(Year(1998), Month(4), Day(1));
    d.AddDays(366);
    ASSERT_EQUAL(d, Date(Year(1999), Month(4), Day(2)))
  }
  // 1999 -> 2000 ; No leap -> leap
  {
    Date d(Year(1999), Month(1), Day(1));
    d.AddDays(365);
    ASSERT_EQUAL(d, Date(Year(2000), Month(1), Day(1)))
  }
  {
    Date d(Year(1999), Month(1), Day(1));
    d.AddDays(366);
    ASSERT_EQUAL(d, Date(Year(2000), Month(1), Day(2)))
  }
  {
    Date d(Year(1999), Month(2), Day(1));
    d.AddDays(365);
    ASSERT_EQUAL(d, Date(Year(2000), Month(2), Day(1)))
  }
  {
    Date d(Year(1999), Month(2), Day(1));
    d.AddDays(366);
    ASSERT_EQUAL(d, Date(Year(2000), Month(2), Day(2)))
  }
  {
    Date d(Year(1999), Month(3), Day(1));
    d.AddDays(365);
    ASSERT_EQUAL(d, Date(Year(2000), Month(2), Day(29)))
  }
  {
    Date d(Year(1999), Month(3), Day(1));
    d.AddDays(366);
    ASSERT_EQUAL(d, Date(Year(2000), Month(3), Day(1)))
  }
  {
    Date d(Year(1999), Month(4), Day(1));
    d.AddDays(365);
    ASSERT_EQUAL(d, Date(Year(2000), Month(3), Day(31)))
  }
  {
    Date d(Year(1999), Month(4), Day(1));
    d.AddDays(366);
    ASSERT_EQUAL(d, Date(Year(2000), Month(4), Day(1)))
  }
  // 2000 -> 2001 ; Leap -> No leap
  {
    Date d(Year(2000), Month(1), Day(1));
    d.AddDays(365);
    ASSERT_EQUAL(d, Date(Year(2000), Month(12), Day(31)))
  }
  {
    Date d(Year(2000), Month(1), Day(1));
    d.AddDays(366);
    ASSERT_EQUAL(d, Date(Year(2001), Month(1), Day(1)))
  }
  {
    Date d(Year(2000), Month(2), Day(1));
    d.AddDays(365);
    ASSERT_EQUAL(d, Date(Year(2001), Month(1), Day(31)))
  }
  {
    Date d(Year(2000), Month(2), Day(1));
    d.AddDays(366);
    ASSERT_EQUAL(d, Date(Year(2001), Month(2), Day(1)))
  }
  {
    Date d(Year(2000), Month(3), Day(1));
    d.AddDays(365);
    ASSERT_EQUAL(d, Date(Year(2001), Month(3), Day(1)))
  }
  {
    Date d(Year(2000), Month(3), Day(1));
    d.AddDays(366);
    ASSERT_EQUAL(d, Date(Year(2001), Month(3), Day(2)))
  }
  {
    Date d(Year(2000), Month(4), Day(1));
    d.AddDays(365);
    ASSERT_EQUAL(d, Date(Year(2001), Month(4), Day(1)))
  }
  {
    Date d(Year(2000), Month(4), Day(1));
    d.AddDays(366);
    ASSERT_EQUAL(d, Date(Year(2001), Month(4), Day(2)))
  }

  // Couple more
  {
    Date d(Year(2019), Month(8), Day(1));
    d.AddDays(113);
    ASSERT_EQUAL(d, Date(Year(2019), Month(11), Day(22)))
  }
  {
    Date d(Year(2019), Month(8), Day(1));
    d.AddDays(219);
    ASSERT_EQUAL(d, Date(Year(2020), Month(3), Day(7)))
  }
}

void TestReadCommands() {
  {
    stringstream s;
    s << "3" << endl;
    s << "Earn 2000-01-02 2000-01-06 20" << endl;
    s << "ComputeIncome 2000-01-01 2001-01-01" << endl;
    s << "PayTax 2000-01-02 2000-01-03" << endl;
    auto commands = ReadCommands(s);
    EarnCommand earn{Date(Year(2000), Month(1), Day(2)),
                     Date(Year(2000), Month(1), Day(6)), 20};
    ComputeIncomeCommand compute{Date(Year(2000), Month(1), Day(1)),
                                 Date(Year(2001), Month(1), Day(1))};
    PayTaxCommand payTax{Date(Year(2000), Month(1), Day(2)),
                         Date(Year(2000), Month(1), Day(3))};
    ASSERT_EQUAL(commands[0]->Kind(), Commands::EarnCommand);
    ASSERT_EQUAL(*commands[0].get(), earn);
    ASSERT_EQUAL(commands[1]->Kind(), Commands::ComputeIncomeCommand);
    ASSERT_EQUAL(*commands[1].get(), compute);
    ASSERT_EQUAL(commands[2]->Kind(), Commands::PayTaxCommand);
    ASSERT_EQUAL(*commands[2].get(), payTax);
  }
}

#define TEST_SUB_DAYS(str)              \
  {                                     \
    istringstream input(str);           \
    Date start, fin365, fin366;         \
    input >> start >> fin365 >> fin366; \
    ASSERT_EQUAL(fin365 - start, 365);  \
    ASSERT_EQUAL(fin366 - start, 366);  \
    ASSERT_EQUAL(start - fin365, -365); \
    ASSERT_EQUAL(start - fin366, -366); \
  }

void TestSubDays() {
  {
    Date start{Year(2000), Month(1), Day(1)};
    Date finish{Year(2000), Month(1), Day(31)};
    DateRange range{start, finish};
    ASSERT_EQUAL(range.Days(), 30);
  }
  {
    Date start{Year(2000), Month(1), Day(1)};
    Date finish{Year(2000), Month(1), Day(31)};
    DateRange range{finish, start};
    ASSERT_EQUAL(range.Days(), -30);
  }
  TEST_SUB_DAYS("1998-01-01	1999-01-01 1999-01-02");
  TEST_SUB_DAYS("1998-02-01	1999-02-01 1999-02-02");
  TEST_SUB_DAYS("1998-03-01	1999-03-01 1999-03-02");
  TEST_SUB_DAYS("1998-04-01	1999-04-01 1999-04-02");
  TEST_SUB_DAYS("1999-01-01	2000-01-01 2000-01-02");
  TEST_SUB_DAYS("1999-02-01	2000-02-01 2000-02-02");
  TEST_SUB_DAYS("1999-03-01	2000-02-29 2000-03-01");
  TEST_SUB_DAYS("1999-04-01	2000-03-31 2000-04-01");
  TEST_SUB_DAYS("2000-01-01	2000-12-31 2001-01-01");
  TEST_SUB_DAYS("2000-02-01	2001-01-31 2001-02-01");
  TEST_SUB_DAYS("2000-03-01	2001-03-01 2001-03-02");
  TEST_SUB_DAYS("2000-04-01	2001-04-01 2001-04-02");
}

void TestDateRange() {
  {
    Date start{Year(2000), Month(1), Day(1)};
    Date finish{Year(2000), Month(1), Day(2)};
    DateRange range{start, finish};
    auto num = 0;
    auto curr = start;
    for (auto d : range) {
      num++;
      ASSERT_EQUAL(d, curr);
      curr.AddDays(1);
    }
    ASSERT_EQUAL(curr, finish.Next());
    ASSERT_EQUAL(num, 2);
  }
  {
    Date start{Year(2000), Month(1), Day(1)};
    Date finish{Year(2000), Month(1), Day(31)};
    DateRange range{start, finish};
    auto num = 0;
    auto curr = start;
    for (auto d : range) {
      num++;
      ASSERT_EQUAL(d, curr);
      curr.AddDays(1);
    }
    ASSERT_EQUAL(curr, finish.Next());
    ASSERT_EQUAL(num, 31);
  }
}

void TestEarnCommand() {
  double count = 10;
  for (int days = 1; days <= 31; days++) {
    CommandProcessor processor;
    EarnCommand command{Date(Year(2000), Month(1), Day(1)),
                        Date(Year(2000), Month(1), Day(days)), count};
    processor.Earn(command);
    auto &earnings = processor.GetEarnings();
    for (int i = 1; i <= days; i++) {
      ASSERT_EQUAL(earnings.at(Date(Year(2000), Month(1), Day(i))),
                   count / days);
    }
  }
}

void TestSample() {
  stringstream s;
  s << "8" << endl
    << "Earn 2000-01-02 2000-01-06 20" << endl
    << "ComputeIncome 2000-01-01 2001-01-01" << endl
    << "PayTax 2000-01-02 2000-01-03" << endl
    << "ComputeIncome 2000-01-01 2001-01-01" << endl
    << "Earn 2000-01-03 2000-01-03 10" << endl
    << "ComputeIncome 2000-01-01 2001-01-01" << endl
    << "PayTax 2000-01-03 2000-01-03" << endl
    << "ComputeIncome 2000-01-01 2001-01-01" << endl;
  auto commands = ReadCommands(s);
  auto res = ProcessCommands(commands);
  vector<string> expected = {"20.000000", "18.960000", "28.960000",
                             "27.207600"};
  ASSERT_EQUAL(res, expected);
}

void RunAllTests() {
  TestRunner tr;
  RUN_TEST(tr, TestDates);
  RUN_TEST(tr, TestDateInc);
  RUN_TEST(tr, TestDateAdditionDays);
  RUN_TEST(tr, TestReadCommands);
  RUN_TEST(tr, TestSubDays);
  RUN_TEST(tr, TestDateRange);
  RUN_TEST(tr, TestEarnCommand);
  RUN_TEST(tr, TestSample);
}

int main() {
  // RunAllTests();
  auto commands = ReadCommands();
  auto output = ProcessCommands(commands);
  for (auto &o : output) {
    cout << o << endl;
  }
  return 0;
}