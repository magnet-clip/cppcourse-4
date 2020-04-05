#pragma once

#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace Json {

class Node;
using JsonArray = std::vector<Node>;
using JsonObject = std::map<std::string, Node>;
using JsonStr = std::string;
using JsonInt = int;
using JsonDouble = double;
using JsonBool = bool;

class Node : std::variant<JsonArray, JsonObject, JsonInt, JsonDouble, JsonStr,
                          JsonBool> {
public:
  using variant::variant;

  std::string ToString(bool prettify = false, int level = 0) const;
  const auto IsArray() const {
    return std::holds_alternative<JsonArray>(*this);
  }
  const auto &AsArray() const { return std::get<JsonArray>(*this); }
  const auto IsBool() const { return std::holds_alternative<JsonBool>(*this); }
  const auto AsBool() const { return std::get<JsonBool>(*this); }
  const auto IsMap() const { return std::holds_alternative<JsonObject>(*this); }
  const auto &AsMap() const { return std::get<JsonObject>(*this); }
  const auto IsInt() const { return std::holds_alternative<JsonInt>(*this); }
  int AsInt() const { return std::get<JsonInt>(*this); }
  const auto IsDouble() const {
    return std::holds_alternative<JsonDouble>(*this);
  }
  double AsDouble() const {
    return std::get<JsonDouble>(*this);
    ;
  }
  const auto IsString() const { return std::holds_alternative<JsonStr>(*this); }
  const auto &AsString() const { return std::get<JsonStr>(*this); }
  const auto IsNumber() const { return IsInt() || IsDouble(); }
  double AsNumber() {
    if (IsInt()) {
      return static_cast<double>(AsInt());
    } else {
      return AsDouble();
    }
  }
};

bool operator==(const Node &a, const Node &b);
std::ostream &operator<<(std::ostream &os, const Node &node);

class Document {
public:
  explicit Document(Node root);

  const Node &GetRoot() const;

private:
  Node root;
};

Document Load(std::istream &input);

} // namespace Json
