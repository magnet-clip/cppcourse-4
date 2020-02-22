#pragma once

#include <string>

enum class HttpCode {
  Ok = 200,
  NotFound = 404,
  Found = 302,
};

std::string GetComment(HttpCode code);