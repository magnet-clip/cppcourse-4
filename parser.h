#pragma once

#include "command.h"
#include "json.h"
#include "query.h"

#include <string>

class Parser {
public:
  CommandPtr ParseCommand(const std::string &command_type);
  QueryPtr ParseQuery(const std::string &query_type);

private:
  virtual BusCommand ParseBusCommand() = 0;
  virtual StopCommand ParseStopCommand() = 0;
  virtual BusQuery ParseBusQuery() = 0;
  virtual StopQuery ParseStopQuery() = 0;
  virtual RouteQuery ParseRouteQuery() = 0;
};

class StringParser : public Parser {
public:
  void SetLine(std::string_view line) { _line = line; }

private:
  std::string_view _line;
  virtual BusCommand ParseBusCommand() override;
  virtual StopCommand ParseStopCommand() override;
  virtual RouteQuery ParseRouteQuery() override;
  virtual BusQuery ParseBusQuery() override;
  virtual StopQuery ParseStopQuery() override;
};

class JsonParser : public Parser {
public:
  void SetNode(const Json::Node &node) { _node = node; }

private:
  Json::Node _node;
  virtual BusCommand ParseBusCommand() override;
  virtual StopCommand ParseStopCommand() override;
  virtual BusQuery ParseBusQuery() override;
  virtual StopQuery ParseStopQuery() override;
  virtual RouteQuery ParseRouteQuery() override;
};