#pragma once

#include <sstream>
#include <string>

struct RouteItemType {
  static constexpr char const *Wait = "Wait";
  static constexpr char const *Bus = "Bus";
};

struct RouteItem {
  double time;
  virtual std::string Kind() const = 0;
};

struct WaitRouteItem : public RouteItem {
  std::string stop_name;
  virtual std::string Kind() const override { return RouteItemType::Wait; };
};

struct BusRouteItem : public RouteItem {
  std::string bus_name;
  int span_count;
  virtual std::string Kind() const override { return RouteItemType::Bus; };
};

std::ostream &operator<<(std::ostream &ss, const BusRouteItem &bus_item);
std::ostream &operator<<(std::ostream &ss, const WaitRouteItem &wait_item);