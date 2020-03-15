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
  static Month Jan;
  static Month Feb;
  static Month Mar;
  static Month Apr;
  static Month May;
  static Month Jun;
  static Month Jul;
  static Month Aug;
  static Month Sep;
  static Month Oct;
  static Month Nov;
  static Month Dec;
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

Month Month::Jan{1};
Month Month::Feb{2};
Month Month::Mar{3};
Month Month::Apr{4};
Month Month::May{5};
Month Month::Jun{6};
Month Month::Jul{7};
Month Month::Aug{8};
Month Month::Sep{9};
Month Month::Oct{10};
Month Month::Nov{11};
Month Month::Dec{12};

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
        if (since.IsLeap()) {
          count++;
        }
        since += 4;
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
    os << Kind() << " " << from << " " << to;
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
    os << Kind() << " " << from << " " << to << " " << value;
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
      : _earnings(LAST_EPOCH_DATE.GetEpochDays(), 0.0),
        _spendings(LAST_EPOCH_DATE.GetEpochDays(), 0.0) {}

  void Earn(const EarnCommand &earn) {
    auto range = DateRange(earn.from, earn.to);
    double days = range.Days() + 1;
    for (const auto &d : range) {
      _earnings[d] += static_cast<double>(earn.value) / days;
    }
  }

  void Spend(const SpendCommand &spend) {
    auto range = DateRange(spend.from, spend.to);
    double days = range.Days() + 1;
    for (const auto &d : range) {
      _spendings[d] += static_cast<double>(spend.value) / days;
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

  optional<double> ProcessCommand(shared_ptr<Command> command) {
    if (command->Kind() == Commands::ComputeIncomeCommand) {
      auto compute = static_cast<ComputeIncomeCommand &>(*command);
      auto amount = ComputeIncome(compute);
      return amount;
    } else if (command->Kind() == Commands::EarnCommand) {
      auto earn = static_cast<EarnCommand &>(*command);
      Earn(earn);
    } else if (command->Kind() == Commands::PayTaxCommand) {
      auto pay = static_cast<PayTaxCommand &>(*command);
      PayTax(pay);
    } else if (command->Kind() == Commands::SpendCommand) {
      auto spend = static_cast<SpendCommand &>(*command);
      Spend(spend);
    }
    return nullopt;
  }

  const vector<double> &GetEarnings() const { return _earnings; }
  const vector<double> &GetSpendings() const { return _spendings; }

 private:
  vector<double> _earnings;
  vector<double> _spendings;
};

vector<double> ProcessCommands(vector<shared_ptr<Command>> commands) {
  CommandProcessor processor;
  vector<double> answers;
  for (auto &command : commands) {
    // cerr << command->ToString();
    auto res = processor.ProcessCommand(command);
    if (res) {
      answers.push_back(res.value());
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
  {
    Date start{Year(2096), Month::Dec, Day(31)};
    ASSERT_EQUAL(start.GetEpochDays(), 35429);
  }
  {
    Date finish{Year(2097), Month::Jan, Day(1)};
    ASSERT_EQUAL(finish.GetEpochDays(), 35430);
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
    Date start{Year(2043), Month::Jan, Day(29)};
    Date finish{Year(2043), Month::Sep, Day(23)};
    ASSERT_EQUAL(finish - start, 237);
  }
  {
    Date start{Year(2003), Month::Dec, Day(11)};
    Date finish{Year(2036), Month::Sep, Day(17)};
    ASSERT_EQUAL(finish - start, 11969);
  }
  {
    Date start{Year(2026), Month::Aug, Day(28)};
    Date finish{Year(2026), Month::Dec, Day(15)};
    ASSERT_EQUAL(finish - start, 109);
  }
  {
    Date start{Year(2026), Month::Aug, Day(28)};
    Date finish{Year(2026), Month::Dec, Day(15)};
    ASSERT_EQUAL(finish - start, 109);
  }
  {
    Date start{Year(2026), Month::Aug, Day(28)};
    Date finish{Year(2027), Month::Feb, Day(3)};
    ASSERT_EQUAL(finish - start, 159);
  }
  {
    Date start{Year(2026), Month::Aug, Day(28)};
    Date finish{Year(2028), Month::Feb, Day(3)};
    ASSERT_EQUAL(finish - start, 524);
  }
  {
    Date start{Year(2026), Month::Aug, Day(28)};
    Date finish{Year(2029), Month::Feb, Day(3)};
    ASSERT_EQUAL(finish - start, 890);
  }
  {
    Date start{Year(2026), Month::Aug, Day(28)};
    Date finish{Year(2030), Month::Feb, Day(3)};
    ASSERT_EQUAL(finish - start, 1255);
  }
  {
    Date start{Year(2026), Month::Aug, Day(28)};
    Date finish{Year(2031), Month::Feb, Day(3)};
    ASSERT_EQUAL(finish - start, 1620);
  }

  {
    Date start{Year(2031), Month::Feb, Day(3)};
    Date finish{Year(2035), Month::Jul, Day(13)};
    ASSERT_EQUAL(finish - start, 1621);
  }
  {
    Date start{Year(2026), Month::Aug, Day(28)};
    Date finish{Year(2035), Month::Jul, Day(13)};
    ASSERT_EQUAL(finish - start, 3241);
  }

  {
    Date start{Year(2035), Month::Jul, Day(13)};
    Date finish{Year(2044), Month::May, Day(27)};
    ASSERT_EQUAL(finish - start, 3241);
  }
  {
    Date start{Year(2026), Month::Aug, Day(28)};
    Date finish{Year(2044), Month::May, Day(27)};
    ASSERT_EQUAL(finish - start, 6482);
  }

  {
    Date start{Year(2044), Month::May, Day(27)};
    Date finish{Year(2062), Month::Feb, Day(25)};
    ASSERT_EQUAL(finish - start, 6483);
  }
  {
    Date start{Year(2026), Month::Aug, Day(28)};
    Date finish{Year(2062), Month::Feb, Day(25)};
    ASSERT_EQUAL(finish - start, 12965);
  }
  {
    Date start{Year(2062), Month::Feb, Day(25)};
    Date finish{Year(2079), Month::Nov, Day(25)};
    ASSERT_EQUAL(finish - start, 6482);
  }
  {
    Date start{Year(2079), Month::Nov, Day(25)};
    Date finish{Year(2088), Month::Oct, Day(9)};
    ASSERT_EQUAL(finish - start, 3241);
  }

  {
    Date start{Year(2088), Month::Oct, Day(9)};
    Date finish{Year(2093), Month::Mar, Day(18)};
    ASSERT_EQUAL(finish - start, 1621);
  }
  {
    Date start{Year(2097), Month::Mar, Day(18)};
    Date finish{Year(2097), Month::Aug, Day(25)};
    ASSERT_EQUAL(finish - start, 160);
  }
  {
    Date start{Year(2096), Month::Mar, Day(18)};
    Date finish{Year(2096), Month::Aug, Day(25)};
    ASSERT_EQUAL(finish - start, 160);
  }
  {
    Date start{Year(2096), Month::Mar, Day(18)};
    Date finish{Year(2096), Month::Dec, Day(5)};
    ASSERT_EQUAL(finish - start, 262);
  }

  {
    Date start{Year(2097), Month::Apr, Day(5)};
    Date finish{Year(2097), Month::Aug, Day(25)};
    ASSERT_EQUAL(finish - start, 142);
  }
  {
    Date start{Year(2097), Month::Jan, Day(5)};
    Date finish{Year(2097), Month::Feb, Day(25)};
    ASSERT_EQUAL(finish - start, 51);
  }
  {
    Date start{Year(2097), Month::Jan, Day(5)};
    Date finish{Year(2097), Month::Jan, Day(25)};
    ASSERT_EQUAL(finish - start, 20);
  }
  {
    Date start{Year(2095), Month::Dec, Day(25)};
    Date finish{Year(2096), Month::Jan, Day(25)};
    ASSERT_EQUAL(finish - start, 31);
  }
  {
    Date start{Year(2096), Month::Dec, Day(31)};
    Date finish{Year(2097), Month::Jan, Day(1)};
    ASSERT_EQUAL(finish - start, 1);
  }
  {
    Date start{Year(2096), Month::Dec, Day(28)};
    Date finish{Year(2097), Month::Jan, Day(2)};
    ASSERT_EQUAL(finish - start, 5);
  }
  {
    Date start{Year(2096), Month::Dec, Day(25)};
    Date finish{Year(2097), Month::Jan, Day(5)};
    ASSERT_EQUAL(finish - start, 11);
  }
  {
    Date start{Year(2096), Month::Dec, Day(25)};
    Date finish{Year(2097), Month::Jan, Day(15)};
    ASSERT_EQUAL(finish - start, 21);
  }
  {
    Date start{Year(2096), Month::Dec, Day(25)};
    Date finish{Year(2097), Month::Jan, Day(25)};
    ASSERT_EQUAL(finish - start, 31);
  }
  {
    Date start{Year(2096), Month::Dec, Day(15)};
    Date finish{Year(2097), Month::Jan, Day(25)};
    ASSERT_EQUAL(finish - start, 41);
  }
  {
    Date start{Year(2096), Month::Dec, Day(5)};
    Date finish{Year(2097), Month::Jan, Day(25)};
    ASSERT_EQUAL(finish - start, 51);
  }

  {
    Date start{Year(2096), Month::Dec, Day(5)};
    Date finish{Year(2097), Month::Feb, Day(25)};
    ASSERT_EQUAL(finish - start, 82);
  }
  {
    Date start{Year(2096), Month::Dec, Day(5)};
    Date finish{Year(2097), Month::Mar, Day(25)};
    ASSERT_EQUAL(finish - start, 110);
  }

  {
    Date start{Year(2096), Month::Dec, Day(5)};
    Date finish{Year(2097), Month::Jul, Day(25)};
    ASSERT_EQUAL(finish - start, 232);
  }
  {
    Date start{Year(2096), Month::Dec, Day(5)};
    Date finish{Year(2097), Month::Aug, Day(25)};
    ASSERT_EQUAL(finish - start, 263);
  }

  {
    Date start{Year(2096), Month::Mar, Day(18)};
    Date finish{Year(2097), Month::Aug, Day(25)};
    ASSERT_EQUAL(finish - start, 525);
  }
  {
    Date start{Year(2095), Month::Mar, Day(18)};
    Date finish{Year(2097), Month::Aug, Day(25)};
    ASSERT_EQUAL(finish - start, 891);
  }
  {
    Date start{Year(2094), Month::Mar, Day(18)};
    Date finish{Year(2097), Month::Aug, Day(25)};
    ASSERT_EQUAL(finish - start, 1256);
  }

  {
    Date start{Year(2093), Month::Mar, Day(18)};
    Date finish{Year(2097), Month::Aug, Day(25)};
    ASSERT_EQUAL(finish - start, 1621);
  }

  {
    Date start{Year(2093), Month::Mar, Day(18)};
    Date finish{Year(2097), Month::Aug, Day(25)};
    ASSERT_EQUAL(finish - start, 1621);
  }
  {
    Date start{Year(2088), Month::Oct, Day(9)};
    Date finish{Year(2097), Month::Aug, Day(25)};
    ASSERT_EQUAL(finish - start, 3242);
  }
  {
    Date start{Year(2079), Month::Nov, Day(25)};
    Date finish{Year(2097), Month::Aug, Day(25)};
    ASSERT_EQUAL(finish - start, 6483);
  }

  {
    Date start{Year(2062), Month::Feb, Day(25)};
    Date finish{Year(2097), Month::Aug, Day(25)};
    ASSERT_EQUAL(finish - start, 12965);
  }
  {
    Date start{Year(2026), Month::Aug, Day(28)};
    Date finish{Year(2097), Month::Sep, Day(25)};
    ASSERT_EQUAL(finish - start, 25961);
  }
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
      ASSERT_EQUAL(Date(d), curr);
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
  vector<double> expected = {20.000000, 18.960000, 8.460000, 8.460000};
  ASSERT_EQUAL(res, expected);
}

void TestSimple() {
  stringstream s;
  s << "4" << endl
    << "Earn 2043-01-29 2046-09-23 126352" << endl
    << "Earn 2026-08-28 2097-08-25 508939" << endl
    << "Earn 2003-12-11 2036-09-17 802587" << endl
    << "ComputeIncome 2051-05-13 2080-09-11" << endl;

  double expected = 210299.6947669321380089968;

  auto commands = ReadCommands(s);
  auto res = ProcessCommands(commands);
  ASSERT_EQUAL(res[0], expected);
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

void TestIt() {
  stringstream s;
  s << "100" << endl
    << "Earn 2043-01-29 2046-09-23 126352" << endl
    << "Earn 2026-08-28 2097-08-25 508939" << endl
    << "Earn 2003-12-11 2036-09-17 802587" << endl
    << "ComputeIncome 2051-05-13 2080-09-11" << endl
    << "Earn 2001-11-11 2015-02-16 430438" << endl
    << "Earn 2002-06-30 2042-08-07 224943" << endl
    << "Earn 2041-05-14 2069-04-18 271131" << endl
    << "ComputeIncome 2008-01-01 2026-01-25" << endl
    << "ComputeIncome 2011-11-05 2035-05-17" << endl
    << "Earn 2025-09-14 2092-10-02 513572" << endl
    << "ComputeIncome 2038-09-08 2062-02-27" << endl
    << "ComputeIncome 2037-10-30 2099-08-23" << endl
    << "Earn 2024-04-13 2025-10-31 260141" << endl
    << "Earn 2039-08-05 2049-02-23 702058" << endl
    << "Earn 2047-09-13 2083-04-02 987762" << endl
    << "Earn 2058-12-27 2064-09-17 187281" << endl
    << "ComputeIncome 2040-08-07 2049-04-09" << endl
    << "Earn 2039-10-11 2080-09-17 271867" << endl
    << "ComputeIncome 2038-09-18 2095-04-21" << endl
    << "Earn 2022-07-16 2091-12-15 154886" << endl
    << "Earn 2032-02-19 2055-04-05 664221" << endl
    << "ComputeIncome 2046-02-07 2081-05-26" << endl
    << "ComputeIncome 2075-06-26 2078-02-15" << endl
    << "ComputeIncome 2060-07-27 2090-11-10" << endl
    << "ComputeIncome 2002-01-26 2046-06-16" << endl
    << "ComputeIncome 2029-10-12 2085-02-15" << endl
    << "Earn 2003-03-13 2028-03-01 515170" << endl
    << "ComputeIncome 2087-11-16 2094-01-19" << endl
    << "Earn 2058-11-06 2077-11-21 578006" << endl
    << "Earn 2059-08-02 2090-04-01 528146" << endl
    << "Earn 2051-08-01 2097-11-27 421841" << endl
    << "ComputeIncome 2010-12-22 2097-12-05" << endl
    << "ComputeIncome 2039-03-20 2086-04-26" << endl
    << "Earn 2007-12-29 2055-02-04 381067" << endl
    << "Earn 2037-10-14 2088-04-06 449243" << endl
    << "Earn 2072-03-23 2073-03-12 864570" << endl
    << "Earn 2012-06-04 2058-08-06 145377" << endl
    << "ComputeIncome 2007-01-31 2059-04-08" << endl
    << "Earn 2074-02-01 2098-03-08 18260" << endl
    << "ComputeIncome 2007-07-01 2062-10-10" << endl
    << "Earn 2071-02-09 2091-05-23 387113" << endl
    << "ComputeIncome 2032-03-12 2085-08-11" << endl
    << "Earn 2017-06-25 2058-08-26 778779" << endl
    << "ComputeIncome 2001-02-10 2087-12-22" << endl
    << "ComputeIncome 2080-02-11 2081-05-04" << endl
    << "ComputeIncome 2019-12-05 2087-09-04" << endl
    << "Earn 2014-07-05 2024-06-28 574400" << endl
    << "ComputeIncome 2011-04-12 2094-03-21" << endl
    << "Earn 2027-05-16 2095-03-02 974252" << endl
    << "Earn 2042-08-29 2080-02-22 992797" << endl
    << "ComputeIncome 2019-02-24 2057-05-23" << endl
    << "ComputeIncome 2041-11-30 2077-11-30" << endl
    << "ComputeIncome 2023-07-01 2088-12-17" << endl
    << "Earn 2024-05-21 2091-11-16 512975" << endl
    << "Earn 2008-09-19 2053-09-22 475187" << endl
    << "ComputeIncome 2018-04-28 2075-01-18" << endl
    << "Earn 2003-12-04 2019-10-20 867020" << endl
    << "Earn 2018-12-21 2027-03-03 912147" << endl
    << "Earn 2036-07-24 2095-02-08 833282" << endl
    << "Earn 2006-03-23 2062-06-25 434021" << endl
    << "ComputeIncome 2007-08-03 2032-11-26" << endl
    << "ComputeIncome 2030-01-07 2038-02-07" << endl
    << "Earn 2077-02-27 2093-07-28 618498" << endl
    << "ComputeIncome 2054-04-16 2088-10-06" << endl
    << "Earn 2028-05-05 2094-01-05 679191" << endl
    << "ComputeIncome 2018-12-14 2046-02-28" << endl
    << "Earn 2003-12-20 2015-08-24 878262" << endl
    << "ComputeIncome 2045-11-28 2055-10-03" << endl
    << "ComputeIncome 2023-01-07 2080-08-29" << endl
    << "Earn 2008-12-02 2057-05-26 808057" << endl
    << "Earn 2088-01-26 2094-03-25 866878" << endl
    << "ComputeIncome 2049-12-20 2064-02-15" << endl
    << "Earn 2022-09-27 2031-09-25 432827" << endl
    << "Earn 2042-10-12 2056-12-09 122540" << endl
    << "Earn 2042-04-27 2061-07-26 974301" << endl
    << "Earn 2061-12-13 2064-04-23 703682" << endl
    << "Earn 2064-10-11 2068-12-05 291068" << endl
    << "ComputeIncome 2071-12-06 2096-06-02" << endl
    << "Earn 2002-09-16 2011-03-10 667572" << endl
    << "ComputeIncome 2064-03-30 2093-02-12" << endl
    << "Earn 2017-08-13 2029-05-18 360789" << endl
    << "ComputeIncome 2013-10-26 2015-02-24" << endl
    << "ComputeIncome 2086-11-06 2091-08-03" << endl
    << "Earn 2011-12-27 2014-05-24 832633" << endl
    << "ComputeIncome 2076-06-25 2080-11-08" << endl
    << "ComputeIncome 2016-12-10 2019-08-12" << endl
    << "Earn 2036-09-06 2085-10-08 481762" << endl
    << "ComputeIncome 2053-08-08 2065-01-11" << endl
    << "Earn 2010-06-12 2049-05-09 505958" << endl
    << "ComputeIncome 2005-01-20 2043-10-09" << endl
    << "Earn 2018-11-29 2080-09-23 134744" << endl
    << "ComputeIncome 2021-07-24 2051-01-31" << endl
    << "ComputeIncome 2077-01-12 2084-11-01" << endl
    << "Earn 2051-09-08 2052-09-05 108031" << endl
    << "Earn 2047-08-19 2048-10-07 313299" << endl
    << "ComputeIncome 2055-09-21 2079-10-24" << endl
    << "ComputeIncome 2062-03-02 2094-10-07" << endl
    << "Earn 2012-01-16 2087-07-31 971809" << endl
    << "Earn 2053-03-01 2097-11-22 990118" << endl
    << "Earn 2026-10-20 2093-02-01 905742" << endl;

  vector<double> expected = {
      210299.6947669321380089968, 775171.6273740400793030858,
      877373.3132458021864295006, 698231.3779814913868904114,
      1273786.90009246370755136,  1014726.685448664939031005,
      3387862.351301584858447313, 2689348.419343071989715099,
      136157.9196220620942767709, 1500974.282768943347036839,
      3192234.287967257667332888, 4396611.253303409554064274,
      90794.64417319600761402398, 7473532.099937264807522297,
      5184636.462666060775518417, 5380851.182088924571871758,
      5900316.653157595545053482, 7469587.107339071109890938,
      10853739.43104400113224983, 126818.9779728063585935161,
      9400999.996262684464454651, 10959310.01754136756062508,
      6074502.035812788642942905, 7569229.10444030724465847,
      11131927.80446905642747879, 10925142.93589833565056324,
      5143169.062981239520013332, 1112997.226670022122561932,
      7375046.835959316231310368, 5967520.385031554847955704,
      2382983.716363547835499048, 13234446.29213313013315201,
      3191923.941770884674042463, 5617441.973064662888646126,
      7173198.431088002398610115, 381827.3237291579716838896,
      1180254.505131053272634745, 948034.3955544924829155207,
      731770.2197613712633028626, 3778879.658197369892150164,
      11710459.2162239532917738,  8752172.286350470036268234,
      1606131.647298994241282344, 7549248.136577017605304718,
      8807740.568511344492435455};

  auto commands = ReadCommands(s);
  CommandProcessor p;
  int res_idx = 0;
  int i = 0;
  for (auto &command : commands) {
    i++;
    auto res = p.ProcessCommand(command);
    if (res) {
      // cerr << i << ") " << command->ToString() << endl;
      ASSERT(fabs(expected[res_idx++] - res.value()) < 0.0000001);
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
  RUN_TEST(tr, TestSimple);
  RUN_TEST(tr, TestIt);
}

int main() {
  // RunAllTests();
  // LoadTest();
  auto commands = ReadCommands();
  auto output = ProcessCommands(commands);
  //  Earn requests 55, ComputeIncome requests 45
  // int num_earn = 0;
  // int num_comp = 0;
  // for (auto &command : commands) {
  //   if (command->Kind() == Commands::ComputeIncomeCommand) num_comp++;
  //   if (command->Kind() == Commands::EarnCommand) num_earn++;
  // }
  // if (num_earn == 55 && num_comp == 45) return 0;
  for (auto &o : output) {
    cout << setprecision(10) << o << endl;
  }
  return 0;
}