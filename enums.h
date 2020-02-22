#pragma once

#include <string>

#define DECL_STATIC(T, Name) static T& Name();

struct HttpMethod {
 private:
  HttpMethod(const std::string name);

 public:
  DECL_STATIC(HttpMethod, GET);
  DECL_STATIC(HttpMethod, POST);

  bool operator==(const std::string& name) const;
  const std::string& GetName() const;

 private:
  const std::string _name;
};

struct ServerPaths {
 private:
  ServerPaths(const std::string name);

 public:
  DECL_STATIC(ServerPaths, Captcha);
  DECL_STATIC(ServerPaths, CheckCaptcha);
  DECL_STATIC(ServerPaths, AddUser);
  DECL_STATIC(ServerPaths, AddComment);
  DECL_STATIC(ServerPaths, UserComments);

  friend bool operator==(const std::string& name, const ServerPaths& method);

  const std::string GetName() const;

 private:
  const std::string _name;
};
struct HttpHeaders {
 private:
  HttpHeaders(const std::string name);

 public:
  DECL_STATIC(HttpHeaders, Location);
  DECL_STATIC(HttpHeaders, ContentLength);

  friend bool operator==(const std::string& name, const HttpHeaders& method);

  const std::string GetName() const;

 private:
  const std::string _name;
};