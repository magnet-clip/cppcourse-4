#include "http_handler.h"
#include <string>
#include "enums.h"
#include "utils.h"

using namespace std;

HttpHandler::HttpHandler(CommentServer* server) : _server(server) {}

NotFound::NotFound() noexcept : HttpHandler(nullptr) {}

bool NotFound::ShouldHandle(const HttpRequestAddress& address) const {
  return false;  // This guarantees that it will be called only on purpose
}

HttpResponse NotFound::operator()(const HttpRequest& req) {
  return HttpResponse(HttpCode::NotFound);
}

AddUserCommand::AddUserCommand(CommentServer* server) : HttpHandler(server) {}
bool AddUserCommand::ShouldHandle(const HttpRequestAddress& address) const {
  return address.method == HttpMethod::POST().GetName() &&
         address.path == ServerPaths::AddUser().GetName();
}
HttpResponse AddUserCommand::operator()(const HttpRequest& req) {
  _server->AddEmptyComment();
  return HttpResponse(HttpCode::Ok)
      .SetContent(std::to_string(_server->GetCommentsCount() - 1));
}

AddCommentCommand::AddCommentCommand(CommentServer* server)
    : HttpHandler(server) {}
bool AddCommentCommand::ShouldHandle(const HttpRequestAddress& address) const {
  return address.method == HttpMethod::POST().GetName() &&
         address.path == ServerPaths::AddComment().GetName();
}

HttpResponse AddCommentCommand::operator()(const HttpRequest& req) {
  auto [user_id, comment] = ParseIdAndContent(req.body);

  auto last_comment = _server->GetLastComment();

  if (!last_comment || last_comment->user_id != user_id) {
    _server->SetLastComment({user_id, 1});
  } else if (_server->UpdateLastCommentCount() > 3) {
    _server->Ban(user_id);
  }
  if (!_server->IsUserBanned(user_id)) {
    _server->AddComment(user_id, std::string(comment));
    return HttpResponse(HttpCode::Ok);
  } else {
    return HttpResponse(HttpCode::Found)
        .AddHeader(HttpHeaders::Location().GetName(),
                   ServerPaths::Captcha().GetName());
  }
}

CheckCaptchaCommmand::CheckCaptchaCommmand(CommentServer* server)
    : HttpHandler(server) {}
bool CheckCaptchaCommmand::ShouldHandle(
    const HttpRequestAddress& address) const {
  return address.method == HttpMethod::POST().GetName() &&
         address.path == ServerPaths::CheckCaptcha().GetName();
}

HttpResponse CheckCaptchaCommmand::operator()(const HttpRequest& req) {
  auto [user_id, response] = ParseIdAndContent(req.body);
  if (response == "42") {
    _server->LiftBan(user_id);
    auto last_comment = _server->GetLastComment();
    if (last_comment != nullptr && last_comment->user_id == user_id) {
      _server->ResetLastComment();
    }
    return HttpResponse(HttpCode::Ok);
  } else {
    return HttpResponse(HttpCode::Found)
        .AddHeader(HttpHeaders::Location().GetName(),
                   ServerPaths::Captcha().GetName());
    ;
  }
}

UserCommentsCommand::UserCommentsCommand(CommentServer* server)
    : HttpHandler(server) {}
bool UserCommentsCommand::ShouldHandle(
    const HttpRequestAddress& address) const {
  return address.method == HttpMethod::GET().GetName() &&
         address.path == ServerPaths::UserComments().GetName();
}

HttpResponse UserCommentsCommand::operator()(const HttpRequest& req) {
  auto user_id = FromString<size_t>(req.get_params.at("user_id"));
  string response;
  for (const string& c : _server->GetUserComments(user_id)) {
    response += c + '\n';
  }

  return HttpResponse(HttpCode::Ok).SetContent(response);
}

CaptchaCommand::CaptchaCommand(CommentServer* server) : HttpHandler(server) {}
bool CaptchaCommand::ShouldHandle(const HttpRequestAddress& address) const {
  return address.method == HttpMethod::GET().GetName() &&
         address.path == ServerPaths::Captcha().GetName();
}

HttpResponse CaptchaCommand::operator()(const HttpRequest& req) {
  return HttpResponse(HttpCode::Ok)
      .SetContent(
          "What's the answer for The Ultimate Question of Life, the "
          "Universe, and Everything?");
}