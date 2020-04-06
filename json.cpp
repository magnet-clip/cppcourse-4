#include "json.h"

#include <cmath>
#include <iostream>

using namespace std;

namespace Json {

Document::Document(Node root) : root(move(root)) {}

const Node &Document::GetRoot() const { return root; }

Node LoadNode(istream &input);

Node LoadArray(istream &input) {
  vector<Node> result;

  for (char c; input >> c && c != ']';) {
    if (c != ',') {
      input.putback(c);
    }
    result.push_back(LoadNode(input));
  }

  return Node(move(result));
}

Node LoadBool(istream &input) {
  char c;
  input >> c;
  bool result = c == 't';

  if (result) {
    input.ignore(3);
  } else {
    input.ignore(4);
  }

  return Node(move(result));
}

Node LoadNumber(istream &input) {
  const int DIGIT_COUNT = 128;
  array<char, DIGIT_COUNT> digits;

  int idx = 0;
  bool its_double = false;
  bool negative = false;
  char symbol = input.get();
  if (symbol == '-') {
    negative = true;
    symbol = input.get();
  }
  while (isdigit(symbol) || symbol == '.') {
    if (symbol == '.') {
      digits[idx] = symbol;
      its_double = true;
    } else {
      digits[idx] = symbol - '0';
    }
    idx++;
    symbol = input.get();
  }
  input.putback(symbol);

  if (its_double) {
    double result = 0.0;
    bool fractions = false;
    double power = 1;
    for (int i = 0; i < idx; i++) {
      if (digits[i] != '.') {
        if (!fractions) {
          result *= 10;
          result += digits[i];
        } else {
          power *= 10;
          result += digits[i] / power;
        }
      } else {
        fractions = true;
      }
    }
    return Node(negative ? -result : result);

  } else {
    int result = 0;
    for (int i = 0; i < idx; i++) {
      result *= 10;
      result += digits[i];
    }
    return Node(negative ? -result : result);
  }
}

Node LoadString(istream &input) {
  string line;
  getline(input, line, '"');

  return Node(move(line));
}

Node LoadDict(istream &input) {
  map<string, Node> result;

  for (char c; input >> c && c != '}';) {
    if (c == ',') {
      input >> c;
    }

    string key = LoadString(input).AsString();
    input >> c;
    result.emplace(move(key), LoadNode(input));
  }

  return Node(move(result));
}

Node LoadNode(istream &input) {
  char c;
  input >> c;

  if (c == '[') {
    return LoadArray(input);
  } else if (c == '{') {
    return LoadDict(input);
  } else if (c == '"') {
    return LoadString(input);
  } else if ((c >= '0' && c <= '9') || (c == '-')) {
    input.putback(c);
    return LoadNumber(input);
  } else {
    input.putback(c);
    return LoadBool(input);
  }
}

string Node::ToString(bool prettify, int level) const {
  stringstream s;
  string tab = prettify ? string(level * 2, ' ') : "";
  string comma = prettify ? ", " : ",";
  string semicolon = prettify ? ": " : ":";
  auto end_line = prettify ? "\n" : "";
  if (IsDouble()) {
    s.precision(7);
    s << AsDouble();
  } else if (IsInt()) {
    s << AsInt();
  } else if (IsBool()) {
    s << (AsBool() ? "true" : "false");
  } else if (IsString()) {
    s << "\"" << AsString() << "\"";
  } else if (IsMap()) {
    const auto &data = AsMap();
    s << "{";
    bool first = true;
    for (const auto &[key, node] : data) {
      if (!first) {
        s << comma;
      } else {
        first = false;
      }
      s << end_line << tab << "\"" << key << "\"" << semicolon
        << node.ToString(prettify, level + 1);
    }
    s << end_line << tab << "}";

  } else if (IsArray()) {
    const auto &data = AsArray();
    bool first = true;
    s << "[";
    for (const auto &node : data) {
      if (!first) {
        s << comma;
      } else {
        first = false;
      }
      s << end_line << tab << node.ToString(prettify, level + 1);
    }
    s << end_line << tab << "]";
  }
  return s.str();
}

bool operator==(const Node &a, const Node &b) {
  if (a.IsInt() && b.IsInt()) {
    return a.AsInt() == b.AsInt();
  } else if (a.IsDouble() && b.IsDouble()) {
    return fabs(a.AsDouble() - b.AsDouble()) < 0.00001;
  } else if (a.IsNumber() && b.IsNumber()) {
    return fabs(a.AsNumber() - b.AsNumber()) < 0.00001;
  } else if (a.IsString() && b.IsString()) {
    return a.AsString() == b.AsString();
  } else if (a.IsBool() && b.IsBool()) {
    return a.AsBool() == b.AsBool();
  } else if (a.IsArray() && b.IsArray()) {
    const auto &a_arr = a.AsArray();
    const auto &b_arr = b.AsArray();
    if (a_arr.size() == b_arr.size()) {
      for (size_t i = 0; i < a_arr.size(); i++) {
        if (!(a_arr[i] == b_arr[i])) {
          return false;
        }
      }
      return true;
    } else {
      return false;
    }
  } else if (a.IsMap() && b.IsMap()) {
    const auto &a_map = a.AsMap();
    const auto &b_map = b.AsMap();
    if (a_map.size() == b_map.size()) {
      for (const auto &[key, value] : a_map) {
        if (!b_map.count(key)) {
          return false;
        }
        if (!(value == b_map.at(key))) {
          return false;
        }
      }
      return true;
    } else {
      return false;
    }
  }
  return false;
}

ostream &operator<<(ostream &os, const Node &node) {
  return os << node.ToString();
}

Document Load(istream &input) { return Document{LoadNode(input)}; }

} // namespace Json
