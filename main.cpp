#include "test_runner.h"

#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

enum class HttpCode {
  Ok = 200,
  NotFound = 404,
  Found = 302,
};

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

#define STATIC(T, Name)   \
  static T& Name() {      \
    static T Name(#Name); \
    return Name;          \
  }

#define STATIC_VAL(T, Name, Value) \
  static T& Name() {               \
    static T Name(Value);          \
    return Name;                   \
  }

struct HttpMethod {
 private:
  HttpMethod(const string name) : _name(name) {}

 public:
  STATIC(HttpMethod, GET);
  STATIC(HttpMethod, POST);

  bool operator==(const string& name) const { return name == _name; }
  friend bool operator==(const string& name, const HttpMethod& method) {
    return name == method._name;
  }
  const string& GetName() const { return _name; }

 private:
  const string _name;
};

struct ServerPaths {
 private:
  ServerPaths(const string name) : _name(name) {}

 public:
  STATIC_VAL(ServerPaths, Captcha, "/captcha");
  STATIC_VAL(ServerPaths, CheckCaptcha, "/checkcaptcha");
  STATIC_VAL(ServerPaths, AddUser, "/add_user");
  STATIC_VAL(ServerPaths, AddComment, "/add_comment");
  STATIC_VAL(ServerPaths, UserComments, "/user_comments");

  bool operator==(const string& name) const { return name == _name; }
  friend bool operator==(const string& name, const ServerPaths& method) {
    return name == method._name;
  }

  const string GetName() const { return _name; }

 private:
  const string _name;
};
struct HttpHeaders {
 private:
  HttpHeaders(const string name) : _name(name) {}

 public:
  STATIC(HttpHeaders, Location);
  STATIC_VAL(HttpHeaders, ContentLength, "Content-Length");

  bool operator==(const string& name) const { return name == _name; }
  friend bool operator==(const string& name, const HttpHeaders& method) {
    return name == method._name;
  }

  const string GetName() const { return _name; }

 private:
  const string _name;
};
class HttpResponse {
 public:
  explicit HttpResponse(HttpCode code) : _code(code) {}

  HttpResponse& AddHeader(string name, string value) {
    _headers.push_back({name, value});
    return *this;
  }
  HttpResponse& SetContent(string a_content) {
    _content = a_content;
    return *this;
  }
  HttpResponse& SetCode(HttpCode a_code) {
    _code = a_code;
    return *this;
  }

  friend ostream& operator<<(ostream& output, const HttpResponse& resp) {
    output << "HTTP/1.1 " << static_cast<int>(resp._code) << " "
           << GetComment(resp._code) << endl;

    for (const auto& [name, value] : resp._headers) {
      output << name << ": " << value << endl;
    }

    if (resp._content.size() != 0) {
      output << HttpHeaders::ContentLength().GetName() << ": "
             << resp._content.size() << endl;
      output << endl;
      output << resp._content << endl;
    }
    return output;
  }

 private:
  HttpCode _code;
  string _content;
  vector<pair<string, string>> _headers;
};

struct HttpRequest {
  string method, path, body;
  map<string, string> get_params;
};

pair<string, string> SplitBy(const string& what, const string& by) {
  size_t pos = what.find(by);
  if (by.size() < what.size() && pos < what.size() - by.size()) {
    return {what.substr(0, pos), what.substr(pos + by.size())};
  } else {
    return {what, {}};
  }
}

template <typename T>
T FromString(const string& s) {
  T x;
  istringstream is(s);
  is >> x;
  return x;
}

pair<size_t, string> ParseIdAndContent(const string& body) {
  auto [id_string, content] = SplitBy(body, " ");
  return {FromString<size_t>(id_string), content};
}

struct LastCommentInfo {
  size_t user_id, consecutive_count;
};
class CommentServer;
struct HttpRequestAddress {
  string method, path;
};
struct HttpHandler {
  HttpHandler(CommentServer* server);
  virtual bool ShouldHandle(const HttpRequestAddress& address) const = 0;
  virtual HttpResponse operator()(const HttpRequest& req) = 0;

 protected:
  CommentServer* _server;
};
class HandlerResolver {
 public:
  HandlerResolver(shared_ptr<HttpHandler> default_handler);
  HandlerResolver& AddHandler(shared_ptr<HttpHandler> handler);
  shared_ptr<HttpHandler> Get(const HttpRequestAddress& address);

 private:
  shared_ptr<HttpHandler> _default;
  list<shared_ptr<HttpHandler>> _handlers;
};

class CommentServer {
 public:
  CommentServer();
  HttpResponse ServeRequest(const HttpRequest& req);

  void AddEmptyComment();
  void AddComment(size_t user_id, const string& comment);
  size_t GetCommentsCount() const;
  const LastCommentInfo* GetLastComment() const;
  void SetLastComment(LastCommentInfo&& info);
  void Ban(size_t user_id);
  int UpdateLastCommentCount();
  size_t IsUserBanned(size_t user_id) const;

 private:
  vector<vector<string>> comments_;
  std::optional<LastCommentInfo> last_comment;
  unordered_set<size_t> banned_users;
  HandlerResolver resolver;
};

class NotFound : public HttpHandler {
 public:
  NotFound() noexcept : HttpHandler(nullptr) {}

  virtual bool ShouldHandle(const HttpRequestAddress& address) const override {
    return false;  // This guarantees that it will be called only on purpose
  }
  virtual HttpResponse operator()(const HttpRequest& req) override {
    return HttpResponse(HttpCode::NotFound);
  }
};

class AddUserCommand : public HttpHandler {
 public:
  AddUserCommand(CommentServer* server) : HttpHandler(server) {}
  virtual bool ShouldHandle(const HttpRequestAddress& address) const override {
    cout << "Comparing {" << address.method << " " << address.path << "} to {"
         << HttpMethod::POST().GetName() << " "
         << ServerPaths::AddUser().GetName() << "}" << endl;
    return address.method == HttpMethod::POST().GetName() &&
           address.path == ServerPaths::AddUser().GetName();
  }
  virtual HttpResponse operator()(const HttpRequest& req) override {
    _server->AddEmptyComment();
    return HttpResponse(HttpCode::Ok)
        .SetContent(to_string(_server->GetCommentsCount() - 1));
  }
};

class AddCommentCommand : public HttpHandler {
 public:
  AddCommentCommand(CommentServer* server) : HttpHandler(server) {}
  virtual bool ShouldHandle(const HttpRequestAddress& address) const override {
    cout << "Comparing {" << address.method << " " << address.path << "} to {"
         << HttpMethod::POST().GetName() << " "
         << ServerPaths::AddComment().GetName() << "}" << endl;
    return address.method == HttpMethod::POST().GetName() &&
           address.path == ServerPaths::AddComment().GetName();
  }
  virtual HttpResponse operator()(const HttpRequest& req) override {
    auto [user_id, comment] = ParseIdAndContent(req.body);

    auto last_comment = _server->GetLastComment();

    if (!last_comment || last_comment->user_id != user_id) {
      _server->SetLastComment({user_id, 1});
    } else if (_server->UpdateLastCommentCount() > 3) {
      _server->Ban(user_id);
    }
    if (!_server->IsUserBanned(user_id)) {
      _server->AddComment(user_id, string(comment));
      return HttpResponse(HttpCode::Ok);
    } else {
      return HttpResponse(HttpCode::Found)
          .AddHeader(HttpHeaders::Location().GetName(),
                     ServerPaths::Captcha().GetName());
    }
  }
};

HttpHandler::HttpHandler(CommentServer* server) : _server(server) {}

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

HttpResponse CommentServer::ServeRequest(const HttpRequest& req) {
  auto handler = resolver.Get({req.method, req.path});
  return (*handler)(req);
}

CommentServer::CommentServer()
    : resolver(HandlerResolver(make_shared<NotFound>(NotFound()))) {
  resolver.AddHandler(make_shared<AddUserCommand>(AddUserCommand(this)))
      .AddHandler(make_shared<AddCommentCommand>(AddCommentCommand(this)));
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

struct HttpHeader {
  string name, value;
};

ostream& operator<<(ostream& output, const HttpHeader& h) {
  return output << h.name << ": " << h.value;
}

bool operator==(const HttpHeader& lhs, const HttpHeader& rhs) {
  return lhs.name == rhs.name && lhs.value == rhs.value;
}

struct ParsedResponse {
  int code;
  vector<HttpHeader> headers;
  string content;
};

istream& operator>>(istream& input, ParsedResponse& r) {
  string line;
  getline(input, line);

  {
    istringstream code_input(line);
    string dummy;
    code_input >> dummy >> r.code;
  }

  size_t content_length = 0;

  r.headers.clear();
  while (getline(input, line) && !line.empty()) {
    if (auto [name, value] = SplitBy(line, ": "); name == "Content-Length") {
      istringstream length_input(value);
      length_input >> content_length;
    } else {
      r.headers.push_back({std::move(name), std::move(value)});
    }
  }

  r.content.resize(content_length);
  input.read(r.content.data(), r.content.size());
  return input;
}

void Test(CommentServer& srv, const HttpRequest& request,
          const ParsedResponse& expected) {
  stringstream ss;
  ss << srv.ServeRequest(request);
  ParsedResponse resp;
  ss >> resp;
  ASSERT_EQUAL(resp.code, expected.code);
  ASSERT_EQUAL(resp.headers, expected.headers);
  ASSERT_EQUAL(resp.content, expected.content);
}

template <typename CommentServer>
void TestServer() {
  CommentServer cs;

  const ParsedResponse ok{200};
  const ParsedResponse redirect_to_captcha{302, {{"Location", "/captcha"}}, {}};
  const ParsedResponse not_found{404};

  Test(cs, {"POST", "/add_user"}, {200, {}, "0"});
  Test(cs, {"POST", "/add_user"}, {200, {}, "1"});
  Test(cs, {"POST", "/add_comment", "0 Hello"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Hi"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Buy my goods"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Enlarge"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Buy my goods"}, redirect_to_captcha);
  Test(cs, {"POST", "/add_comment", "0 What are you selling?"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Buy my goods"}, redirect_to_captcha);
  Test(cs, {"GET", "/user_comments", "", {{"user_id", "0"}}},
       {200, {}, "Hello\nWhat are you selling?\n"});
  Test(cs, {"GET", "/user_comments", "", {{"user_id", "1"}}},
       {200, {}, "Hi\nBuy my goods\nEnlarge\n"});
  Test(cs, {"GET", "/captcha"},
       {200,
        {},
        {"What's the answer for The Ultimate Question of Life, the Universe, "
         "and Everything?"}});
  Test(cs, {"POST", "/checkcaptcha", "1 24"}, redirect_to_captcha);
  Test(cs, {"POST", "/checkcaptcha", "1 42"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Sorry! No spam any more"}, ok);
  Test(cs, {"GET", "/user_comments", "", {{"user_id", "1"}}},
       {200, {}, "Hi\nBuy my goods\nEnlarge\nSorry! No spam any more\n"});

  Test(cs, {"GET", "/user_commntes"}, not_found);
  Test(cs, {"POST", "/add_uesr"}, not_found);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestServer<CommentServer>);
}
