#pragma once

#include "geomath.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

struct Commands {
  static std::string StopCommand;
};

struct Command {
  virtual std::string Kind() const = 0;
  virtual std::string ToString() const = 0;
  virtual bool operator==(const Command &other) const = 0;
};

struct StopCommand : public Command {
  StopCommand(std::string name, GeoPoint location)
      : _name(name), _location(location) {}
  virtual std::string Kind() const override { return Commands::StopCommand; }
  virtual std::string ToString() const override;
  virtual bool operator==(const Command &other) const override;

private:
  std::string _name;
  GeoPoint _location;
};

std::vector<std::shared_ptr<Command>> ReadCommands(std::istream &is = std::cin);