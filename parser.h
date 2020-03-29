#pragma once

#include "command.h"
#include "query.h"

#include <string>

class Parser {
public:
  CommandPtr ParseCommand(const std::string& command_type,
                          std::string_view line) const;
  QueryPtr ParseQuery(const std::string &query_type,
                      std::string_view line) const;

private:
  virtual BusCommand ParseBusCommand(std::string_view line) const = 0;
  virtual StopCommand ParseStopCommand(std::string_view line) const = 0;
  virtual BusQuery ParseBusQuery(std::string_view line) const = 0;
  virtual StopQuery ParseStopQuery(std::string_view line) const = 0;
};

class StringParser : public Parser {
private:
  virtual BusCommand ParseBusCommand(std::string_view line) const override;
  virtual StopCommand ParseStopCommand(std::string_view line) const override;
  virtual BusQuery ParseBusQuery(std::string_view line) const override;
  virtual StopQuery ParseStopQuery(std::string_view line) const override;
};

class JsonParser : public Parser {
private:
  virtual BusCommand ParseBusCommand(std::string_view line) const override;
  virtual StopCommand ParseStopCommand(std::string_view line) const override;
  virtual BusQuery ParseBusQuery(std::string_view line) const override;
  virtual StopQuery ParseStopQuery(std::string_view line) const override;
};