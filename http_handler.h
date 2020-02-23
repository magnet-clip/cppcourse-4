#pragma once

#include "comment_server.h"
#include "http_response.h"

class CommentServer;

struct HttpHandler {
  HttpHandler(CommentServer* server);
  virtual bool ShouldHandle(const HttpRequestAddress& address) const = 0;
  virtual HttpResponse operator()(const HttpRequest& req) = 0;

 protected:
  CommentServer* _server;
};

class NotFound : public HttpHandler {
 public:
  NotFound() noexcept;

  virtual bool ShouldHandle(const HttpRequestAddress& address) const override;
  virtual HttpResponse operator()(const HttpRequest& req) override;
};

class AddUserCommand : public HttpHandler {
 public:
  AddUserCommand(CommentServer* server);
  virtual bool ShouldHandle(const HttpRequestAddress& address) const override;
  virtual HttpResponse operator()(const HttpRequest& req) override;
};

class AddCommentCommand : public HttpHandler {
 public:
  AddCommentCommand(CommentServer* server);
  virtual bool ShouldHandle(const HttpRequestAddress& address) const override;

  virtual HttpResponse operator()(const HttpRequest& req) override;
};

class CheckCaptchaCommmand : public HttpHandler {
 public:
  CheckCaptchaCommmand(CommentServer* server);
  virtual bool ShouldHandle(const HttpRequestAddress& address) const override;

  virtual HttpResponse operator()(const HttpRequest& req) override;
};

class UserCommentsCommand : public HttpHandler {
 public:
  UserCommentsCommand(CommentServer* server);
  virtual bool ShouldHandle(const HttpRequestAddress& address) const override;

  virtual HttpResponse operator()(const HttpRequest& req) override;
};

class CaptchaCommand : public HttpHandler {
 public:
  CaptchaCommand(CommentServer* server);
  virtual bool ShouldHandle(const HttpRequestAddress& address) const override;
  virtual HttpResponse operator()(const HttpRequest& req) override;
};