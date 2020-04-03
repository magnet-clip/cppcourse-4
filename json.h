#pragma once

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#define READ1(var_name, type) const auto var_name = std::get<type>(*this)
#define READ2(var_name, type)                                                  \
  type var_name;                                                               \
  try {                                                                        \
    var_name = std::get<type>(*this);                                          \
  } catch (std::exception e) {                                                 \
    std::cerr << "Error reading type " #type << std::endl;                     \
    throw;                                                                     \
  }

#define READ READ1
namespace Json {

class Node;
using JsonArray = std::vector<Node>;
using JsonObject = std::map<std::string, Node>;
using JsonStr = std::string;

class Node : std::variant<JsonArray, JsonObject, int, double, JsonStr, bool> {
public:
  using variant::variant;

  std::string ToString(bool prettify = false, int level = 0) const;
  const auto IsArray() const {
    return std::holds_alternative<JsonArray>(*this);
  }
  const auto AsArray() const {
    READ(data, JsonArray);
    return std::move(data);
  }
  const auto IsBool() const { return std::holds_alternative<bool>(*this); }
  const auto AsBool() const {
    READ(data, bool);
    return data;
  }
  const auto IsMap() const { return std::holds_alternative<JsonObject>(*this); }
  const auto AsMap() const {
    READ(data, JsonObject);
    return std::move(data);
  }
  const auto IsInt() const { return std::holds_alternative<int>(*this); }
  int AsInt() const {
    READ(data, int);
    return data;
  }
  const auto IsDouble() const { return std::holds_alternative<double>(*this); }
  double AsDouble() const {
    READ(data, double);
    return data;
  }
  const auto IsString() const { return std::holds_alternative<JsonStr>(*this); }
  const auto AsString() const {
    READ(data, JsonStr);
    return std::move(data);
  }
};

class Document {
public:
  explicit Document(Node root);

  const Node &GetRoot() const;

private:
  Node root;
};

Document Load(std::istream &input);

} // namespace Json
