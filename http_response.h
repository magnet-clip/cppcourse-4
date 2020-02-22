#pragma once

#include <ostream>
#include <string>
#include <vector>

#include "utils.h"

class HttpResponse {
 public:
  explicit HttpResponse(HttpCode code);
  HttpResponse& AddHeader(std::string name, std::string value);

  HttpResponse& SetContent(std::string a_content);
  HttpResponse& SetCode(HttpCode a_code);
  friend std::ostream& operator<<(std::ostream& output,
                                  const HttpResponse& resp);

 private:
  HttpCode _code;
  std::string _content;
  std::vector<std::pair<std::string, std::string>> _headers;
};
