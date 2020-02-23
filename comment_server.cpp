#include "comment_server.h"

using namespace std;

CommentServer::CommentServer()
    : resolver(HandlerResolver(make_shared<NotFound>(NotFound()))) {
  resolver.AddHandler(make_shared<AddUserCommand>(AddUserCommand(this)))
      .AddHandler(make_shared<AddCommentCommand>(AddCommentCommand(this)))
      .AddHandler(make_shared<CheckCaptchaCommmand>(CheckCaptchaCommmand(this)))
      .AddHandler(make_shared<UserCommentsCommand>(UserCommentsCommand(this)))
      .AddHandler(make_shared<CaptchaCommand>(CaptchaCommand(this)));
}

void CommentServer::AddEmptyComment() { comments_.emplace_back(); }

void CommentServer::AddComment(size_t user_id, const string& comment) {
  comments_[user_id].push_back(move(comment));
}

size_t CommentServer::GetCommentsCount() const { return comments_.size(); }

const LastCommentInfo* CommentServer::GetLastComment() const {
  return last_comment.has_value() ? &last_comment.value() : nullptr;
}

void CommentServer::SetLastComment(LastCommentInfo&& info) {
  last_comment = move(info);
}

void CommentServer::Ban(size_t user_id) { banned_users.insert(user_id); }

int CommentServer::UpdateLastCommentCount() {
  return ++last_comment->consecutive_count;
}
size_t CommentServer::IsUserBanned(size_t user_id) const {
  return banned_users.count(user_id);
}

void CommentServer::LiftBan(size_t user_id) { banned_users.erase(user_id); }

void CommentServer::ResetLastComment() { last_comment.reset(); }

const vector<string>& CommentServer::GetUserComments(size_t user_id) const {
  return comments_[user_id];
}

HttpResponse CommentServer::ServeRequest(const HttpRequest& req) {
  auto handler = resolver.Get({req.method, req.path});
  return (*handler)(req);
}
