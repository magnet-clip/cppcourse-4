#include "test_runner.h"

#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std;

struct Year {
  explicit Year(int year) : _year(year) {}
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

private:
  int _year;
};

// Months are started from 1
struct Month {
  explicit Month(int month) : _month(month) {
    if (month <= 0)
      throw invalid_argument("month is <= 0");
    if (month > 12)
      throw invalid_argument("month is > 12");
  }
  operator int() const { return _month; }
  Month &operator=(const Month &other) {
    _month = other._month;
    return *this;
  }

private:
  int _month;
};

// Days are started from 1
struct Day {
  explicit Day(int day) : _day(day) {
    if (day <= 0)
      throw invalid_argument("day is <= 0");
  }
  operator int() const { return _day; }
  Day &operator=(const Day &other) {
    _day = other._day;
    return *this;
  }

private:
  int _day;
};

struct Date {
  static const array<int, 12> MonthLengths;

  Date() : year(Year(0)), month(Month(1)), day(Day(1)) {}

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

  Year year;
  Month month;
  Day day;

private:
  Date Next() {
    int month_pos = month - 1;
    int days_in_current_month =
        MonthLengths[month_pos] + ((month == 2 && year.IsLeap()) ? 1 : 0);
    int new_day = day + 1;
    int new_month = month;
    int new_year = year;
    if (new_day > days_in_current_month) {
      new_day = 1;
      new_month += 1;
      if (new_month > 12) {
        new_month = 1;
        new_year += 1;
      }
    }
    return Date(Year(new_year), Month(new_month), Day(new_day));
  }
};

const array<int, 12> Date::MonthLengths = {31, 28, 31, 30, 31, 30,
                                           31, 31, 30, 31, 30, 31};

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

bool operator<(const Date &d1, const Date &d2) {
  return make_tuple(d1.year, d1.month, d1.day) <
         make_tuple(d2.year, d2.month, d2.day);
}

ostream &operator<<(ostream &os, const Date &date) {
  return os << setfill('0') << setw(4) << date.year << '-' << setw(2)
            << date.month << '-' << setw(2) << date.day;
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
  explicit DateRangeIterator(Date &date) : _date(date) {}
  DateRangeIterator &operator++() {
    _date++;
    return *this;
  }
  Date &operator*() { return _date; }

private:
  Date _date;
};

bool operator==(DateRangeIterator &d1, DateRangeIterator &d2) {
  return d1 == d2;
}
bool operator!=(DateRangeIterator &d1, DateRangeIterator &d2) {
  return d1 != d2;
}
class DateRange {
public:
  DateRange(Date from, Date to) : _from(from), _to(to) {}
  int days() const;
  DateRangeIterator begin() const;
  DateRangeIterator end() const;

private:
  Date _from, _to;
};

class CommandProcessor {
public:
  optional<string> ProcessCommand(const EarnCommand &earn) {
    auto range = DateRange(earn.from, earn.to);
    double days = range.days();
    for (const auto &d : range) {
      _earnings[d] += earn.value / days;
    }
    return nullopt;
  }
  optional<string> ProcessCommand(const ComputeIncomeCommand &compute) {
    return nullopt;
  }
  optional<string> ProcessCommand(const PayTaxCommand &compute) {
    return nullopt;
  }

private:
  map<Date, double> _earnings;
};

vector<string> ProcessCommands(vector<shared_ptr<Command>> commands) {
  CommandProcessor processor;
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
    ASSERT_EQUAL(*commands[0].get(), earn);
    ASSERT_EQUAL(commands[0]->Kind(), Commands::EarnCommand);
    ASSERT_EQUAL(*commands[1].get(), compute);
    ASSERT_EQUAL(commands[1]->Kind(), Commands::ComputeIncomeCommand);
    ASSERT_EQUAL(*commands[2].get(), payTax);
    ASSERT_EQUAL(commands[2]->Kind(), Commands::PayTaxCommand);
  }
}

void TestDateRange() {}

void RunAllTests() {
  TestRunner tr;
  RUN_TEST(tr, TestDates);
  RUN_TEST(tr, TestReadCommands);
  RUN_TEST(tr, TestDateRange);
}

int main() {
  RunAllTests();
  auto commands = ReadCommands();
  auto output = ProcessCommands(commands);
  return 0;
}