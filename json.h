#pragma once

#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace Json {

class Node : std::variant<std::vector<Node>, std::map<std::string, Node>, int,
                          double, std::string, bool> {
public:
  using variant::variant;

  std::string ToString(int level = 0) const;
  const auto &AsArray() const { return std::get<std::vector<Node>>(*this); }
  const auto &AsBool() const { return std::get<bool>(*this); }
  const auto &AsMap() const {
    return std::get<std::map<std::string, Node>>(*this);
  }
  int AsInt() const { return std::get<int>(*this); }
  double AsDouble() const { return std::get<double>(*this); }
  const auto &AsString() const { return std::get<std::string>(*this); }
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
