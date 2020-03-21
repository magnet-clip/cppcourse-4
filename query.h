#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct Queries {
  static std::string BusQuery;
};

struct Query {
  virtual std::string Kind() const = 0;
  virtual std::string ToString() const = 0;
  virtual bool operator==(const Query &other) const = 0;
};

class BusQuery : public Query {
public:
  BusQuery(std::string_view line);
  BusQuery(int number) : _number(number) {}
  virtual std::string Kind() const override { return Queries::BusQuery; };
  virtual std::string ToString() const override;
  virtual bool operator==(const Query &other) const override;
  int GetNumber() const { return _number; }

private:
  int _number;
};

std::vector<std::shared_ptr<Query>> ReadQueries(std::istream &is = std::cin);
