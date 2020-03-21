#pragma once
#include <string>

class Responses {
  static std::string BusResponse;
};

struct Response {
  virtual std::string Kind() const = 0;
  virtual std::string ToString() const = 0;
};

struct BusResponse : public Response {
  int bus_number;
  int num_stops;
  int num_unique_stops;
  double length;
  virtual std::string Kind() const override { return Responses::BusResponse; }
  virtual std::string ToString() const override;
};