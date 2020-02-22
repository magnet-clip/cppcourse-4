#include "enums.h"

using namespace std;

#define STATIC(T, Name)   \
  T& T::Name() {          \
    static T Name(#Name); \
    return Name;          \
  }

#define STATIC_VAL(T, Name, Value) \
  T& T::Name() {                   \
    static T Name(Value);          \
    return Name;                   \
  }

HttpMethod::HttpMethod(const string name) : _name(name) {}

STATIC(HttpMethod, GET);
STATIC(HttpMethod, POST);

bool operator==(const string& name, const HttpMethod& method) {
  return name == method.GetName();
}
const string& HttpMethod::GetName() const { return _name; }

ServerPaths::ServerPaths(const string name) : _name(name) {}

STATIC_VAL(ServerPaths, Captcha, "/captcha");
STATIC_VAL(ServerPaths, CheckCaptcha, "/checkcaptcha");
STATIC_VAL(ServerPaths, AddUser, "/add_user");
STATIC_VAL(ServerPaths, AddComment, "/add_comment");
STATIC_VAL(ServerPaths, UserComments, "/user_comments");

bool operator==(const string& name, const ServerPaths& method) {
  return name == method.GetName();
}

const string ServerPaths::GetName() const { return _name; }

HttpHeaders::HttpHeaders(const string name) : _name(name) {}

STATIC(HttpHeaders, Location);
STATIC_VAL(HttpHeaders, ContentLength, "Content-Length");

bool operator==(const string& name, const HttpHeaders& method) {
  return name == method.GetName();
}

const string HttpHeaders::GetName() const { return _name; }
