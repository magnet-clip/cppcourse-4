#include "http_response.h"
#include "enums.h"

using namespace std;

HttpResponse::HttpResponse(HttpCode code) : _code(code) {}

HttpResponse& HttpResponse::AddHeader(string name, string value) {
  _headers.push_back({name, value});
  return *this;
}

HttpResponse& HttpResponse::SetContent(string a_content) {
  _content = a_content;
  return *this;
}

HttpResponse& HttpResponse::SetCode(HttpCode a_code) {
  _code = a_code;
  return *this;
}

ostream& operator<<(ostream& output, const HttpResponse& resp) {
  output << "HTTP/1.1 " << static_cast<int>(resp._code) << " "
         << GetComment(resp._code) << endl;

  for (const auto& [name, value] : resp._headers) {
    output << name << ": " << value << endl;
  }

  if (resp._content.size() != 0) {
    output << HttpHeaders::ContentLength().GetName() << ": "
           << resp._content.size() << endl;
    output << endl;
    output << resp._content;
  } else {
    output << endl;
  }
  return output;
}
