#include "json.h"

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

  if (holds_alternative<double>(*this)) {
    s.precision(7);
    s << get<double>(*this);
  } else if (holds_alternative<int>(*this)) {
    s << get<int>(*this);
  } else if (holds_alternative<bool>(*this)) {
    s << (get<bool>(*this) ? "true" : "false");
  } else if (holds_alternative<string>(*this)) {
    s << "\"" << get<string>(*this) << "\"";
  } else if (holds_alternative<map<string, Node>>(*this)) {
    const auto &data = get<map<string, Node>>(*this);
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

  } else if (holds_alternative<vector<Node>>(*this)) {
    const auto &data = get<vector<Node>>(*this);
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

Document Load(istream &input) { return Document{LoadNode(input)}; }

} // namespace Json
