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
  Query(size_t id) : _id(id) {}
  virtual std::string Kind() const = 0;
  size_t GetId() const { return _id; };

protected:
  size_t _id = 0;
};

using QueryPtr = std::shared_ptr<Query>;

class BusQuery : public Query {
public:
  BusQuery(const std::string &name, size_t id = 0) : Query(id), _name(name) {}
  virtual std::string Kind() const override { return Queries::BusQuery; };
  std::string GetName() const { return _name; }

private:
  std::string _name;
};

class StopQuery : public Query {
public:
  StopQuery(const std::string &name, size_t id = 0) : Query(id), _name(name) {}
  virtual std::string Kind() const override { return Queries::StopQuery; };
  std::string GetName() const { return _name; }

private:
  std::string _name;
};
