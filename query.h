#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct Queries {
  static constexpr char const *BusQuery = "Bus";
  static constexpr char const *StopQuery = "Stop";
};

struct Query {
  virtual std::string Kind() const = 0;
};

using QueryPtr = std::shared_ptr<Query>;

class BusQuery : public Query {
public:
  BusQuery(const std::string &name) : _name(name) {}
  virtual std::string Kind() const override { return Queries::BusQuery; };
  std::string GetName() const { return _name; }

private:
  std::string _name;
};

class StopQuery : public Query {
public:
  StopQuery(const std::string &name) : _name(name) {}
  virtual std::string Kind() const override { return Queries::StopQuery; };
  std::string GetName() const { return _name; }

private:
  std::string _name;
};
