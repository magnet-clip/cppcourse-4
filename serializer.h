#pragma once

#include "response.h"

#include <string>

class Serializer {
public:
  std::string Serialize(ResponsePtr response) const;

private:
  virtual std::string Serialize(const NoBusResponse &response) const = 0;
  virtual std::string Serialize(const FoundBusResponse &response) const = 0;
  virtual std::string Serialize(const NoStopResponse &response) const = 0;
  virtual std::string Serialize(const FoundStopResponse &response) const = 0;
};

class StringSerializer : public Serializer {
private:
  virtual std::string Serialize(const NoBusResponse &response) const override;
  virtual std::string
  Serialize(const FoundBusResponse &response) const override;
  virtual std::string Serialize(const NoStopResponse &response) const override;
  virtual std::string
  Serialize(const FoundStopResponse &response) const override;
};

class JsonSerializer : public Serializer {
private:
  virtual std::string Serialize(const NoBusResponse &response) const override;
  virtual std::string
  Serialize(const FoundBusResponse &response) const override;
  virtual std::string Serialize(const NoStopResponse &response) const override;
  virtual std::string
  Serialize(const FoundStopResponse &response) const override;
};
