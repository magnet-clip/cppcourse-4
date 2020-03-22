#pragma once
#include <string>

struct Responses {
  static std::string FoundBusResponse;
  static std::string NoBusResponse;
};

struct Response {
  virtual std::string Kind() const = 0;
  virtual std::string ToString() const = 0;
};

struct BusResponse : public Response {
  std::string bus_number;
  virtual std::string Kind() const = 0;
  virtual std::string ToString() const = 0;
};

struct FoundBusResponse : public BusResponse {
  int num_stops;
  int num_unique_stops;
  double length;

  virtual std::string Kind() const override {
    return Responses::FoundBusResponse;
  }
  virtual std::string ToString() const override;
};

struct NoBusResponse : public BusResponse {
  NoBusResponse(std::string number) { bus_number = number; }
  virtual std::string Kind() const override { return Responses::NoBusResponse; }
  virtual std::string ToString() const override;
};