#include "handler_resolver.h"

using namespace std;

HandlerResolver::HandlerResolver(shared_ptr<HttpHandler> default_handler)
    : _default(default_handler) {}

HandlerResolver& HandlerResolver::AddHandler(shared_ptr<HttpHandler> handler) {
  _handlers.push_back(handler);
  return *this;
}

shared_ptr<HttpHandler> HandlerResolver::Get(
    const HttpRequestAddress& address) {
  for (auto it = _handlers.begin(); it != _handlers.end(); it++) {
    if ((*it)->ShouldHandle(address)) {
      return *it;
    }
  }
  return _default;
}
