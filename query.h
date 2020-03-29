#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct Queries {
  static std::string BusQuery;
  static std::string StopQuery;
};

struct Query {
  virtual std::string Kind() const = 0;
  virtual bool operator==(const Query &other) const = 0;
};

using QueryPtr = std::shared_ptr<Query>;

class BusQuery : public Query {
public:
  BusQuery(const std::string &name) : _name(name) {}
  virtual std::string Kind() const override { return Queries::BusQuery; };
  virtual bool operator==(const Query &other) const override;
  std::string GetName() const { return _name; }

private:
  std::string _name;
};

class StopQuery : public Query {
public:
  StopQuery(const std::string &name) : _name(name) {}
  virtual std::string Kind() const override { return Queries::StopQuery; };
  virtual bool operator==(const Query &other) const override;
  std::string GetName() const { return _name; }

private:
  std::string _name;
};
