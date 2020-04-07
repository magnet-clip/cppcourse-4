#pragma once
#include "id.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct Queries {
  static constexpr char const *BusQuery = "Bus";
  static constexpr char const *StopQuery = "Stop";
  static constexpr char const *RouteQuery = "Route";
};

struct Query {
  Query(RequestId id) : _id(id) {}
  virtual std::string Kind() const = 0;
  RequestId GetId() const { return _id; };

protected:
  RequestId _id = 0;
};

using QueryPtr = std::shared_ptr<Query>;

class BusQuery : public Query {
public:
  BusQuery(const std::string &name, RequestId id = 0)
      : Query(id), _name(name) {}
  virtual std::string Kind() const override { return Queries::BusQuery; };
  std::string GetName() const { return _name; }

private:
  std::string _name;
};

class StopQuery : public Query {
public:
  StopQuery(const std::string &name, RequestId id = 0)
      : Query(id), _name(name) {}
  virtual std::string Kind() const override { return Queries::StopQuery; };
  std::string GetName() const { return _name; }

private:
  std::string _name;
};

class RouteQuery : public Query {
public:
  RouteQuery(const std::string &from, const std::string &to, RequestId id = 0)
      : Query(id), _from(from), _to(to) {}
  virtual std::string Kind() const override { return Queries::RouteQuery; };
  std::string GetFrom() const { return _from; }
  std::string GetTo() const { return _to; }

private:
  std::string _from;
  std::string _to;
};
