#include "profile.h"
#include "test_runner.h"

#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

// Months start from 1
struct Month {
  explicit Month(int month) : _month(month) {
    if (month <= 0) throw invalid_argument("month is <= 0");
    if (month > 12) throw invalid_argument("month is > 12");
  }
  Month(const Month &other) : _month(other._month) {}
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
    if (day <= 0) {
      throw invalid_argument("day is <= 0");
    }
  }
  Day(const Day &other) : _day(other._day) {}
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

  Year &operator+=(int num) {
    _year += num;
    return *this;
  }

  Year Next() { return Year(_year + 1); }

 private:
  static const array<int, 12> MonthLengths;
  int _year;
};

const array<int, 12> Year::MonthLengths = {31, 28, 31, 30, 31, 30,
                                           31, 31, 30, 31, 30, 31};

const Year EPOCH_START_YEAR{2000};
struct Date {
  Date() : year(Year(EPOCH_START_YEAR)), month(Month(1)), day(Day(1)) {
    if (day > year.DaysInMonth(month)) {
      throw invalid_argument("day too large");
    }
    RecalculateEpochDays();
  }

  explicit Date(int _epoch_days)
      : epoch_days(_epoch_days),
        year(Year(EPOCH_START_YEAR)),
        month(Month(1)),
        day(Day(1)) {
    RecalculateYearMonthDay();
  }

  Date(Year new_year, Month new_month, Day new_day)
      : year(new_year), month(new_month), day(new_day) {
    if (day > year.DaysInMonth(month)) {
      throw invalid_argument("day too large");
    }
    RecalculateEpochDays();
  }

  Date(const Date &other)
      : epoch_days(other.epoch_days),
        year(other.year),
        month(other.month),
        day(other.day) {}

  Date &operator=(const Date &other) {
    year = other.year;
    month = other.month;
    day = other.day;
    epoch_days = other.epoch_days;
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

  void AddDays(int add_days) { *this = Date(epoch_days + add_days); }
  Date Next() const { return Date(epoch_days + 1); }
  Date Prev() const { return Date(epoch_days - 1); }

  const Year GetYear() const { return year; }
  const Month GetMonth() const { return month; }
  const Day GetDay() const { return day; }
  int GetEpochDays() const { return epoch_days; }

 private:
  int epoch_days;
  Year year;
  Month month;
  Day day;

  int NumLeapYears(Year since, Year till) {
    int count = 0;
    while (!since.IsLeap() && since < till) {
      since++;
    }
    if (since < till) {
      // count += (till - since) / 4;
      // TODO: try [count += (till-since)/4] (dates must be in
      // 2000-2099 range). BTW I can simplify IsLeap() check
      do {
        since += 4;
        if (since.IsLeap()) {
          count++;
        }
      } while (since < till);
    }
    return count;
  }

  int NumYearDays() {
    int years = year - EPOCH_START_YEAR;
    return years * 365 + NumLeapYears(EPOCH_START_YEAR, year);
  }

  int NumMonthDays() {
    int days = 0;
    for (int num = 1; num < month; num++) {
      days += year.DaysInMonth(Month(num));
    }
    return days;
  }

  void RecalculateEpochDays() {
    int days = NumYearDays();
    days += NumMonthDays();
    days += day - 1;
    epoch_days = days;
  }

  void RecalculateYearMonthDay() {
    int days_left = epoch_days;

    Year current_year = EPOCH_START_YEAR;
    do {
      auto current_year_days = current_year.DaysInYear();
      if (days_left >= current_year_days) {
        days_left -= current_year_days;
        current_year++;
      } else {
        break;
      }
    } while (true);
    year = current_year;

    Month current_month{1};
    do {
      auto current_month_days = year.DaysInMonth(current_month);
      if (days_left >= current_month_days) {
        days_left -= current_month_days;
        current_month = current_month.NextMonth();
      } else {
        break;
      }
    } while (true);

    day = Day(days_left + 1);
  }
};

static const Date LAST_EPOCH_DATE = Date(Year(2099), Month(12), Day(31));

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

bool operator==(const Date &d, const int days) {
  return d.GetEpochDays() == days;
}

bool operator!=(const Date &d, const int days) {
  return d.GetEpochDays() != days;
}

bool operator==(const Date &d1, const Date &d2) {
  return d1.GetEpochDays() == d2.GetEpochDays();
}

bool operator!=(const Date &d1, const Date &d2) {
  return d1.GetEpochDays() != d2.GetEpochDays();
}

bool operator<(const Date &d1, const Date &d2) {
  return d1.GetEpochDays() < d2.GetEpochDays();
}

bool operator>(const Date &d1, const Date &d2) {
  return d1.GetEpochDays() > d2.GetEpochDays();
}

ostream &operator<<(ostream &os, const Date &date) {
  return os << setfill('0') << setw(4) << date.GetYear() << '-' << setw(2)
            << date.GetMonth() << '-' << setw(2) << date.GetDay();
}

// d1 is expected to be greater than d2
int operator-(Date d1, Date d2) {
  return d1.GetEpochDays() - d2.GetEpochDays();
}

Date operator+(Date d, int days) { return Date(d.GetEpochDays() + days); }
Date operator-(Date d, int days) { return Date(d.GetEpochDays() - days); }

struct Commands {
  static string ComputeIncomeCommand;
  static string EarnCommand;
  static string PayTaxCommand;
  static string SpendCommand;
};

string Commands::ComputeIncomeCommand = "ComputeIncome";
string Commands::EarnCommand = "Earn";
string Commands::PayTaxCommand = "PayTax";
string Commands::SpendCommand = "Spend";

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

struct MoneyCommand : public Command {
  MoneyCommand(Date from, Date to, int new_value)
      : Command(from, to), value(new_value) {}
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
      auto &earn_other = static_cast<const MoneyCommand &>(other);
      return value == earn_other.value;
    }
  }
  int value;
};

struct PayTaxCommand : public MoneyCommand {
  using MoneyCommand::MoneyCommand;
  virtual string Kind() const override { return Commands::PayTaxCommand; }
};

struct EarnCommand : public MoneyCommand {
  using MoneyCommand::MoneyCommand;
  virtual string Kind() const override { return Commands::EarnCommand; }
};

struct SpendCommand : public MoneyCommand {
  using MoneyCommand::MoneyCommand;
  virtual string Kind() const override { return Commands::SpendCommand; }
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
      int value;
      is >> value;
      res.push_back(make_shared<PayTaxCommand>(from, to, value));
    } else if (command == Commands::SpendCommand) {
      int value;
      is >> value;
      res.push_back(make_shared<SpendCommand>(from, to, value));
    }
  }
  return res;
}

class DateRangeIterator {
 public:
  explicit DateRangeIterator(int epoch_days) : _epoch_days(epoch_days) {}
  DateRangeIterator &operator++() {
    _epoch_days++;
    return *this;
  }
  int &operator*() { return _epoch_days; }

 private:
  int _epoch_days;
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
  DateRangeIterator begin() const {
    return DateRangeIterator(_from.GetEpochDays());
  }
  DateRangeIterator end() const {
    return DateRangeIterator(_to.Next().GetEpochDays());
  }

 private:
  Date _from, _to;
};

class CommandProcessor {
 public:
  CommandProcessor()
      : _earnings(LAST_EPOCH_DATE.GetEpochDays(), 0),
        _spendings(LAST_EPOCH_DATE.GetEpochDays(), 0) {}

  void Earn(const EarnCommand &earn) {
    auto range = DateRange(earn.from, earn.to);
    double days = range.Days() + 1;
    for (const auto &d : range) {
      _earnings[d] += earn.value / days;
    }
  }

  void Spend(const SpendCommand &spend) {
    auto range = DateRange(spend.from, spend.to);
    double days = range.Days() + 1;
    for (const auto &d : range) {
      _spendings[d] += spend.value / days;
    }
  }

  double ComputeIncome(const ComputeIncomeCommand &compute) {
    auto range = DateRange(compute.from, compute.to);
    double sum = 0;
    for (const auto &d : range) {
      sum += _earnings[d] - _spendings[d];
    }
    return sum;
  }

  void PayTax(const PayTaxCommand &pay) {
    auto range = DateRange(pay.from, pay.to);
    for (const auto &d : range) {
      _earnings[d] *= (1.0 - static_cast<double>(pay.value) / 100.0);
    }
  }

  const vector<double> &GetEarnings() const { return _earnings; }
  const vector<double> &GetSpendings() const { return _spendings; }

 private:
  vector<double> _earnings;
  vector<double> _spendings;
};

vector<string> ProcessCommands(vector<shared_ptr<Command>> commands) {
  CommandProcessor processor;
  vector<string> answers;
  for (auto &command : commands) {
    if (command->Kind() == Commands::ComputeIncomeCommand) {
      auto compute = static_cast<ComputeIncomeCommand &>(*command);
      auto amount = processor.ComputeIncome(compute);
      answers.push_back(to_string(amount));
    } else if (command->Kind() == Commands::EarnCommand) {
      auto earn = static_cast<EarnCommand &>(*command);
      processor.Earn(earn);
    } else if (command->Kind() == Commands::PayTaxCommand) {
      auto pay = static_cast<PayTaxCommand &>(*command);
      processor.PayTax(pay);
    } else if (command->Kind() == Commands::SpendCommand) {
      auto spend = static_cast<SpendCommand &>(*command);
      processor.Spend(spend);
    }
  }
  return answers;
}

void TestDates() {
  {
    stringstream s;
    s << "2000-01-01";
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
  {
    try {
      Date d(Year(2011), Month(4), Day(100));
      ASSERT(false);
    } catch (...) {
    }
  }
}

void TestDateInc() {
  {
    Date d;
    d++;
    ASSERT_EQUAL(d, Date(Year(2000), Month(1), Day(2)));
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
}

void TestReadCommands() {
  {
    stringstream s;
    s << "3" << endl;
    s << "Earn 2000-01-02 2000-01-06 20" << endl;
    s << "ComputeIncome 2000-01-01 2001-01-01" << endl;
    s << "PayTax 2000-01-02 2000-01-03 20" << endl;
    auto commands = ReadCommands(s);
    EarnCommand earn{Date(Year(2000), Month(1), Day(2)),
                     Date(Year(2000), Month(1), Day(6)), 20};
    ComputeIncomeCommand compute{Date(Year(2000), Month(1), Day(1)),
                                 Date(Year(2001), Month(1), Day(1))};
    PayTaxCommand payTax{Date(Year(2000), Month(1), Day(2)),
                         Date(Year(2000), Month(1), Day(3)), 20};
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

#define TEST_ADD_DAYS(str)              \
  {                                     \
    istringstream input(str);           \
    Date start, fin365, fin366;         \
    input >> start >> fin365 >> fin366; \
    ASSERT_EQUAL(start + 365, fin365);  \
    ASSERT_EQUAL(start + 366, fin366);  \
    ASSERT_EQUAL(fin365 - 365, start);  \
    ASSERT_EQUAL(fin366 - 366, start);  \
  }

void TestAddDays() {
  TEST_ADD_DAYS("2002-01-01	2003-01-01 2003-01-02");
  TEST_ADD_DAYS("2002-02-01	2003-02-01 2003-02-02");
  TEST_ADD_DAYS("2002-03-01	2003-03-01 2003-03-02");
  TEST_ADD_DAYS("2002-04-01	2003-04-01 2003-04-02");
  TEST_ADD_DAYS("2003-01-01	2004-01-01 2004-01-02");
  TEST_ADD_DAYS("2003-02-01	2004-02-01 2004-02-02");
  TEST_ADD_DAYS("2003-03-01	2004-02-29 2004-03-01");
  TEST_ADD_DAYS("2003-04-01	2004-03-31 2004-04-01");
  TEST_ADD_DAYS("2004-01-01	2004-12-31 2005-01-01");
  TEST_ADD_DAYS("2004-02-01	2005-01-31 2005-02-01");
  TEST_ADD_DAYS("2004-03-01	2005-03-01 2005-03-02");
  TEST_ADD_DAYS("2004-04-01	2005-04-01 2005-04-02");
}

void TEST_RANGE_COUNTER(string str) {
  istringstream input(str);
  Date start, fin365, fin366;
  input >> start >> fin365 >> fin366;
  DateRange d365(start, fin365);
  DateRange d366(start, fin366);
  ASSERT_EQUAL(d365.Days(), 365);
  ASSERT_EQUAL(d366.Days(), 366);
  {
    auto num = -1;
    Date d = start;
    for (auto i : d365) {
      num++;
      d.AddDays(1);
    }
    ASSERT_EQUAL(num, 365);
    ASSERT_EQUAL(d, *d365.end());
  }
  {
    auto num = -1;
    Date d = start;
    for (auto i : d366) {
      num++;
      d.AddDays(1);
    }
    ASSERT_EQUAL(num, 366);
    ASSERT_EQUAL(d, *d366.end());
  }
}

void TestRangeCounters() {
  TEST_RANGE_COUNTER("2002-01-01 2003-01-01 2003-01-02");
  TEST_RANGE_COUNTER("2002-02-01 2003-02-01 2003-02-02");
  TEST_RANGE_COUNTER("2002-03-01 2003-03-01 2003-03-02");
  TEST_RANGE_COUNTER("2002-04-01 2003-04-01 2003-04-02");
  TEST_RANGE_COUNTER("2003-01-01 2004-01-01 2004-01-02");
  TEST_RANGE_COUNTER("2003-02-01 2004-02-01 2004-02-02");
  TEST_RANGE_COUNTER("2003-03-01 2004-02-29 2004-03-01");
  TEST_RANGE_COUNTER("2003-04-01 2004-03-31 2004-04-01");
  TEST_RANGE_COUNTER("2004-01-01 2004-12-31 2005-01-01");
  TEST_RANGE_COUNTER("2004-02-01 2005-01-31 2005-02-01");
  TEST_RANGE_COUNTER("2004-03-01 2005-03-01 2005-03-02");
  TEST_RANGE_COUNTER("2004-04-01 2005-04-01 2005-04-02");
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
      ASSERT_EQUAL(d, curr.GetEpochDays());
      curr.AddDays(1);
    }
    ASSERT_EQUAL(curr, *range.end());
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
      ASSERT_EQUAL(d, curr.GetEpochDays());
      curr.AddDays(1);
    }
    ASSERT_EQUAL(curr, finish.Next());
    ASSERT_EQUAL(num, 31);
  }
}

void TestEarnCommand() {
  int count = 10;
  for (int days = 1; days <= 31; days++) {
    CommandProcessor processor;
    EarnCommand command{Date(Year(2000), Month(1), Day(1)),
                        Date(Year(2000), Month(1), Day(days)), count};
    processor.Earn(command);
    auto &earnings = processor.GetEarnings();
    for (int i = 1; i <= days; i++) {
      ASSERT_EQUAL(earnings[Date(Year(2000), Month(1), Day(i)).GetEpochDays()],
                   static_cast<double>(count) / days);
    }
  }
}

void TestSpendCommand() {
  int count = 10;
  for (int days = 1; days <= 31; days++) {
    CommandProcessor processor;
    SpendCommand command{Date(Year(2000), Month(1), Day(1)),
                         Date(Year(2000), Month(1), Day(days)), count};
    processor.Spend(command);
    auto &earnings = processor.GetSpendings();
    for (int i = 1; i <= days; i++) {
      ASSERT_EQUAL(earnings[Date(Year(2000), Month(1), Day(i)).GetEpochDays()],
                   static_cast<double>(count) / days);
    }
  }
}

void TestEarnAndSpendCommand() {
  for (int count = 10; count < 100; count += 10) {
    for (int days = 1; days <= 31; days++) {
      CommandProcessor processor;
      EarnCommand earn{Date(Year(2000), Month(1), Day(1)),
                       Date(Year(2000), Month(1), Day(days)), count};
      SpendCommand spend{Date(Year(2000), Month(1), Day(1)),
                         Date(Year(2000), Month(1), Day(days)), count};
      ComputeIncomeCommand comp(Date(Year(2000), Month(1), Day(1)),
                                Date(Year(2000), Month(1), Day(days)));
      PayTaxCommand pay{Date(Year(2000), Month(1), Day(1)),
                        Date(Year(2000), Month(1), Day(days)), count};
      processor.Earn(earn);
      processor.Spend(spend);
      ASSERT_EQUAL(processor.ComputeIncome(comp), 0);
      processor.PayTax(pay);
      auto res = processor.ComputeIncome(comp);
      ASSERT(fabs(res + count * count / 100.0) < 0.00000001);
    }
  }
}

void TestSample() {
  stringstream s;
  s << "8" << endl
    << "Earn 2000-01-02 2000-01-06 20" << endl
    << "ComputeIncome 2000-01-01 2001-01-01" << endl
    << "PayTax 2000-01-02 2000-01-03 13" << endl
    << "ComputeIncome 2000-01-01 2001-01-01" << endl
    << "Spend 2000-12-30 2001-01-02 14" << endl
    << "ComputeIncome 2000-01-01 2001-01-01" << endl
    << "PayTax 2000-12-30 2000-12-30 13" << endl
    << "ComputeIncome 2000-01-01 2001-01-01" << endl;

  auto commands = ReadCommands(s);
  auto res = ProcessCommands(commands);
  vector<string> expected = {"20.000000", "18.960000", "8.460000", "8.460000"};
  ASSERT_EQUAL(res, expected);
}

void LoadTest() {
  stringstream s;
  s << "8" << endl
    << "Earn 2000-01-02 2000-01-06 20" << endl
    << "ComputeIncome 2000-01-01 2001-01-01" << endl
    << "PayTax 2000-01-02 2000-01-03 13" << endl
    << "ComputeIncome 2000-01-01 2001-01-01" << endl
    << "Spend 2000-12-30 2001-01-02 14" << endl
    << "ComputeIncome 2000-01-01 2001-01-01" << endl
    << "PayTax 2000-12-30 2000-12-30 13" << endl
    << "ComputeIncome 2000-01-01 2001-01-01" << endl;

  auto commands = ReadCommands(s);
  {
    LOG_DURATION("10k");
    for (auto i = 0; i < 100'000; i++) {
      ProcessCommands(commands);
    }
  }
}

void RunAllTests() {
  TestRunner tr;
  RUN_TEST(tr, TestDates);
  RUN_TEST(tr, TestDateInc);
  RUN_TEST(tr, TestReadCommands);
  RUN_TEST(tr, TestSubDays);
  RUN_TEST(tr, TestAddDays);
  RUN_TEST(tr, TestDateRange);
  RUN_TEST(tr, TestRangeCounters);
  RUN_TEST(tr, TestEarnCommand);
  RUN_TEST(tr, TestSpendCommand);
  RUN_TEST(tr, TestEarnAndSpendCommand);
  RUN_TEST(tr, TestSample);
}

int main() {
  // RunAllTests();
  // LoadTest();
  auto commands = ReadCommands();
  auto output = ProcessCommands(commands);
  for (auto &o : output) {
    cout << o << endl;
  }
  return 0;
}