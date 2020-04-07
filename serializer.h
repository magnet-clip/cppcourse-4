#pragma once

#include "json.h"
#include "response.h"

#include <iostream>
#include <memory>
#include <string>

using str = std::string;

class Serializer {
public:
  virtual str Serialize(ResponsePtr response) const;

private:
  virtual str Serialize(const NoBusResponse &response) const = 0;
  virtual str Serialize(const FoundBusResponse &response) const = 0;
  virtual str Serialize(const NoStopResponse &response) const = 0;
  virtual str Serialize(const FoundStopResponse &response) const = 0;
  virtual str Serialize(const NoRouteResponse &response) const = 0;
  virtual str Serialize(const FoundRouteResponse &response) const = 0;
};

using SerializerPtr = std::shared_ptr<Serializer>;

class StringSerializer : public Serializer {
private:
  virtual str Serialize(const NoBusResponse &response) const override;
  virtual str Serialize(const FoundBusResponse &response) const override;
  virtual str Serialize(const NoStopResponse &response) const override;
  virtual str Serialize(const FoundStopResponse &response) const override;
  virtual str Serialize(const NoRouteResponse &response) const override;
  virtual str Serialize(const FoundRouteResponse &response) const override;
};

Json::Node GetJsonNode(const NoBusResponse &response);
Json::Node GetJsonNode(const FoundBusResponse &response);
Json::Node GetJsonNode(const NoStopResponse &response);
Json::Node GetJsonNode(const FoundStopResponse &response);
Json::Node GetJsonNode(const NoRouteResponse &response);
Json::Node GetJsonNode(const FoundRouteResponse &response);

class JsonSerializer : public Serializer {

private:
  virtual str Serialize(const NoBusResponse &response) const override {
    return GetJsonNode(response).ToString();
  }
  virtual str Serialize(const FoundBusResponse &response) const override {
    return GetJsonNode(response).ToString();
  }
  virtual str Serialize(const NoStopResponse &response) const override{
    return GetJsonNode(response).ToString();
  }
  virtual str Serialize(const FoundStopResponse &response) const override{
    return GetJsonNode(response).ToString();
  }
  virtual str Serialize(const NoRouteResponse &response) const override {
    return GetJsonNode(response).ToString();
  }
  virtual str Serialize(const FoundRouteResponse &response) const override {
    return GetJsonNode(response).ToString();
  }
};
