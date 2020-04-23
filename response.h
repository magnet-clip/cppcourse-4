#pragma once

#include "id.h"
#include "map_stop.h"
#include "route_item.h"

#include <memory>
#include <string>
#include <vector>

struct Responses {
  static constexpr char const *FoundBusResponse = "FoundBusResponse";
  static constexpr char const *NoBusResponse = "NoBusResponse";
  static constexpr char const *FoundStopResponse = "FoundStopResponse";
  static constexpr char const *NoStopResponse = "NoStopResponse";
  static constexpr char const *FoundRouteResponse = "FoundRouteResponse";
  static constexpr char const *NoRouteResponse = "NoRouteResponse";
};

struct Response {
  Response(RequestId id) : _id(id) {}
  virtual std::string Kind() const = 0;
  int GetId() const { return _id; }

protected:
  RequestId _id;
};

using ResponsePtr = std::shared_ptr<Response>;

struct BusResponse : public Response {
  BusResponse(RequestId id) : Response(id) {}
  std::string bus_name;
  virtual std::string Kind() const = 0;
};

struct FoundBusResponse : public BusResponse {
  FoundBusResponse(RequestId id) : BusResponse(id) {}
  int num_stops;
  int num_unique_stops;
  double length;
  double curvature;

  virtual std::string Kind() const override {
    return Responses::FoundBusResponse;
  }
};

struct NoBusResponse : public BusResponse {
  NoBusResponse(RequestId id, std::string name) : BusResponse(id) {
    bus_name = name;
  }
  virtual std::string Kind() const override { return Responses::NoBusResponse; }
};

struct StopResponse : public Response {
  StopResponse(RequestId id) : Response(id) {}
  std::string stop_name;
  virtual std::string Kind() const = 0;
};

struct FoundStopResponse : public StopResponse {
  FoundStopResponse(RequestId id) : StopResponse(id) {}
  std::vector<std::string> bus_names;

  virtual std::string Kind() const override {
    return Responses::FoundStopResponse;
  }
};

struct NoStopResponse : public StopResponse {
  NoStopResponse(RequestId id, std::string name) : StopResponse(id) {
    stop_name = name;
  }
  virtual std::string Kind() const override {
    return Responses::NoStopResponse;
  }
};

struct RouteResponse : public Response {
  RouteResponse(RequestId id) : Response(id) {}
  virtual std::string Kind() const = 0;
};

struct FoundRouteResponse : public RouteResponse {
  FoundRouteResponse(RequestId id) : RouteResponse(id) {}
  std::vector<std::shared_ptr<RouteItem>> items;
  double total_time;
  virtual std::string Kind() const override {
    return Responses::FoundRouteResponse;
  }
};

struct NoRouteResponse : public RouteResponse {
  NoRouteResponse(RequestId id) : RouteResponse(id) {}
  virtual std::string Kind() const override {
    return Responses::NoRouteResponse;
  }
};
