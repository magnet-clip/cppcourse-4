#pragma once

#include <memory>
#include <string>
#include <vector>

struct Responses {
  static std::string FoundBusResponse;
  static std::string NoBusResponse;
  static std::string FoundStopResponse;
  static std::string NoStopResponse;
};

struct Response {
  virtual std::string Kind() const = 0;
};

using ResponsePtr = std::shared_ptr<Response>;

struct BusResponse : public Response {
  std::string bus_number;
  virtual std::string Kind() const = 0;
};

struct FoundBusResponse : public BusResponse {
  int num_stops;
  int num_unique_stops;
  double length;
  double curvature;

  virtual std::string Kind() const override {
    return Responses::FoundBusResponse;
  }
};

struct NoBusResponse : public BusResponse {
  NoBusResponse(std::string number) { bus_number = number; }
  virtual std::string Kind() const override { return Responses::NoBusResponse; }
};

struct StopResponse : public Response {
  std::string stop_name;
  virtual std::string Kind() const = 0;
};

struct FoundStopResponse : public StopResponse {
  std::vector<std::string> bus_names;

  virtual std::string Kind() const override {
    return Responses::FoundStopResponse;
  }
};

struct NoStopResponse : public StopResponse {
  NoStopResponse(std::string name) { stop_name = name; }
  virtual std::string Kind() const override {
    return Responses::NoStopResponse;
  }
};
