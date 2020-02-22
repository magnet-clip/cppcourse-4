#include "utils.h"

using namespace std;

string GetComment(HttpCode code) {
  switch (code) {
    case HttpCode::Ok:
      return "OK";
    case HttpCode::NotFound:
      return "Not found";
    case HttpCode::Found:
      return "Found";
  }
}