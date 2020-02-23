#pragma once

#include <list>
#include <memory>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "http_handler.h"
#include "http_response.h"
#include "utils.h"

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

class CommentServer {
 public:
  CommentServer();
  HttpResponse ServeRequest(const HttpRequest& req);

  void AddEmptyComment();
  void AddComment(std::size_t user_id, const std::string& comment);
  std::size_t GetCommentsCount() const;
  const LastCommentInfo* GetLastComment() const;
  void SetLastComment(LastCommentInfo&& info);
  void Ban(std::size_t user_id);
  int UpdateLastCommentCount();
  std::size_t IsUserBanned(std::size_t user_id) const;
  void LiftBan(std::size_t user_id);
  void ResetLastComment();
  const std::vector<std::string>& GetUserComments(std::size_t user_id) const;

 private:
  std::vector<std::vector<std::string>> comments_;
  std::optional<LastCommentInfo> last_comment;
  std::unordered_set<size_t> banned_users;
  HandlerResolver resolver;
};