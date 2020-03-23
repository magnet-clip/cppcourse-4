#pragma once

#include "geomath.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Commands {
  static std::string StopCommand;
  static std::string BusCommand;
};

struct Command {
  virtual std::string Kind() const = 0;
  virtual std::string ToString() const = 0;
  virtual bool operator==(const Command &other) const = 0;
};

class StopCommand : public Command {
public:
  StopCommand(std::string_view line);
  StopCommand(std::string name, GeoPoint location)
      : _name(name), _location(location) {}
  virtual std::string Kind() const override { return Commands::StopCommand; }
  virtual std::string ToString() const override;
  virtual bool operator==(const Command &other) const override;
  std::string GetName() const { return _name; }
  GeoPoint GetLocation() const { return _location; }
  const std::unordered_map<std::string, double> &GetDistances() const {
    return _distances;
  }

private:
  void AddDistance(std::string_view distance_info);
  std::string _name;
  GeoPoint _location;
  std::unordered_map<std::string, double> _distances;
};

class BusCommand : public Command {
public:
  BusCommand(std::string_view line);
  BusCommand(std::string name, std::vector<std::string> stops)
      : _name(name), _circular(false), _stops(stops) {}
  virtual std::string Kind() const override { return Commands::BusCommand; }
  virtual std::string ToString() const override;
  virtual bool operator==(const Command &other) const override;
  std::string GetName() const { return _name; }
  std::vector<std::string> GetStops() const { return _stops; };
  bool IsCircular() const { return _circular; };

private:
  void AddStop(std::string_view str);
  std::string _name;
  bool _circular;
  std::vector<std::string> _stops;
};

using CommandPtr = std::shared_ptr<Command>;

std::vector<CommandPtr> ReadCommands(std::istream &is = std::cin);