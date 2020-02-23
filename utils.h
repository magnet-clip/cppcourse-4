#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

enum class HttpCode {
  Ok = 200,
  NotFound = 404,
  Found = 302,
};
std::string GetComment(HttpCode code);

struct LastCommentInfo {
  size_t user_id, consecutive_count;
};

struct HttpRequestAddress {
  std::string method, path;
};

struct HttpRequest {
  std::string method, path, body;
  std::map<std::string, std::string> get_params;
};

struct HttpHeader {
  std::string name, value;
};

std::ostream& operator<<(std::ostream& output, const HttpHeader& h) {
  return output << h.name << ": " << h.value;
}

bool operator==(const HttpHeader& lhs, const HttpHeader& rhs) {
  return lhs.name == rhs.name && lhs.value == rhs.value;
}

struct ParsedResponse {
  int code;
  std::vector<HttpHeader> headers;
  std::string content;
};

std::istream& operator>>(std::istream& input, ParsedResponse& r);

std::pair<std::string, std::string> SplitBy(const std::string& what,
                                            const std::string& by);

std::pair<size_t, std::string> ParseIdAndContent(const std::string& body);

template <typename T>
T FromString(const std::string& s) {
  T x;
  std::istringstream is(s);
  is >> x;
  return x;
}
