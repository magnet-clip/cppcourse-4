#pragma once

#include "http_handler.h"
#include "utils.h"

#include <list>
#include <memory>

class HttpHandler;

class HandlerResolver {
 public:
  HandlerResolver(std::shared_ptr<HttpHandler> default_handler);
  HandlerResolver& AddHandler(std::shared_ptr<HttpHandler> handler);
  std::shared_ptr<HttpHandler> Get(const HttpRequestAddress& address);

 private:
  std::shared_ptr<HttpHandler> _default;
  std::list<std::shared_ptr<HttpHandler>> _handlers;
};