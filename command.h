#pragma once

#include "geomath.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct Commands {
  static constexpr char const *StopCommand = "Stop";
  static constexpr char const *BusCommand = "Bus";
};

struct Command {
  virtual std::string Kind() const = 0;
};

using CommandPtr = std::shared_ptr<Command>;

class StopCommandBuilder;
class StopCommand : public Command {
  friend class StopCommandBuilder;

public:
  virtual std::string Kind() const override { return Commands::StopCommand; }

  const std::string &GetName() const { return _name; }
  GeoPoint GetLocation() const { return _location; }
  const std::unordered_map<std::string, double> &GetDistances() const {
    return _distances;
  }

private:
  StopCommand(std::string name, GeoPoint location,
              const std::vector<std::pair<std::string, double>> &distances)
      : _name(name), _location(location),
        _distances(distances.begin(), distances.end()) {}

  std::string _name;
  GeoPoint _location;
  std::unordered_map<std::string, double> _distances;
};

class StopCommandBuilder {
public:
  StopCommandBuilder &SetName(const std::string_view &name) {
    _name = std::string(name);
    return *this;
  }
  StopCommandBuilder &SetLocation(GeoPoint location) {
    _location = location;
    return *this;
  }
  StopCommandBuilder &AddDistance(const std::string &name, double distance) {
    _distances.push_back({name, distance});
    return *this;
  }
  StopCommand Build() { return StopCommand(*_name, *_location, _distances); }

private:
  std::optional<std::string> _name;
  std::optional<GeoPoint> _location;
  std::vector<std::pair<std::string, double>> _distances;
};

class BusCommandBuilder;

class BusCommand : public Command {
  friend class BusCommandBuilder;

public:
  virtual std::string Kind() const override { return Commands::BusCommand; }

  const std::string &GetName() const { return _name; }
  const std::vector<std::string> &GetStops() const { return _stops; };
  bool IsCircular() const { return _circular; };

private:
  BusCommand(std::string name, bool circular,
             const std::vector<std::string> &stops)
      : _name(name), _circular(circular), _stops(stops) {}
  std::string _name;
  bool _circular;
  std::vector<std::string> _stops;
};

class BusCommandBuilder {
public:
  BusCommandBuilder(const std::string &name) : _name(name) {}
  BusCommandBuilder &SetIsCircular(bool circular) {
    _circular = circular;
    return *this;
  }
  BusCommandBuilder &AddStop(const std::string_view &name) {
    _stops.push_back(std::string(name));
    return *this;
  }
  BusCommand Build() { return BusCommand(_name, *_circular, _stops); }

private:
  std::string _name;
  std::optional<bool> _circular;
  std::vector<std::string> _stops;
};