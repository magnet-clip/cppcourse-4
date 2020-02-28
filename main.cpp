#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

struct Email {
  string from;
  string to;
  string body;
};

enum class EmailMode { From, To, Body };
EmailMode NextMode(EmailMode mode) {
  return static_cast<EmailMode>((static_cast<int>(mode) + 1) % 3);
}

ostream &operator<<(ostream &os, Email email) {
  os << email.from << endl;
  os << email.to << endl;
  os << email.body << endl;
  return os;
}

class Worker {
public:
  virtual ~Worker() = default;
  virtual void Process(unique_ptr<Email> email) = 0;
  virtual void Run();

protected:
  void PassOn(unique_ptr<Email> email) const;

public:
  void SetNext(unique_ptr<Worker> next);

private:
  unique_ptr<Worker> _next;
};

void Worker::Run() { throw logic_error("Unimplemented"); }

void Worker::SetNext(unique_ptr<Worker> next) { _next = move(next); }

void Worker::PassOn(unique_ptr<Email> email) const {
  if (_next) {
    _next->Process(move(email));
  }
}

class Reader : public Worker {
public:
  Reader(istream &is);
  virtual void Run() override;
  virtual void Process(unique_ptr<Email> email) override;
  virtual ~Reader() = default;

private:
  istream &&_is;
};

Reader::Reader(istream &is) : _is(move(is)) {}

void Reader::Run() {
  EmailMode mode = EmailMode::From;
  string from, to, body;
  for (string line; getline(_is, line);) {
    if (mode == EmailMode::From) {
      from = move(line);
    } else if (mode == EmailMode::To) {
      to = move(line);
    } else if (mode == EmailMode::Body) {
      auto *email = new Email{move(from), move(to), move(line)};
      auto email_ptr = unique_ptr<Email>(email);
      PassOn(move(email_ptr));
    }
    mode = NextMode(mode);
  }
}

void Reader::Process(unique_ptr<Email> email) {
  // do nothing here, possibly should throw an exception as this is undesired
  // behaviour to call this method directly
}

class Filter : public Worker {
public:
  using Function = function<bool(const Email &)>;
  virtual void Process(unique_ptr<Email> email) override;
  virtual ~Filter() = default;

public:
  Filter(Function filter) : _filter(filter) {}

private:
  Function _filter;
};

void Filter::Process(unique_ptr<Email> email) {
  // TODO
}

class Copier : public Worker {
public:
  Copier(string address) : _address(address) {}
  virtual void Process(unique_ptr<Email> email) override;
  virtual ~Copier() = default;

private:
  string _address;
};

void Copier::Process(unique_ptr<Email> email) {
  // TODO
}

class Sender : public Worker {
public:
  Sender(ostream &out);
  virtual void Process(unique_ptr<Email> email) override;
  virtual ~Sender() = default;

private:
  ostream &&_out;
};

Sender::Sender(ostream &out) : _out(move(out)) {}

void Sender::Process(unique_ptr<Email> email) { _out << *email; }

class PipelineBuilder {
public:
  // добавляет в качестве первого обработчика Reader
  explicit PipelineBuilder(istream &in);

  // добавляет новый обработчик Filter
  PipelineBuilder &FilterBy(Filter::Function filter);

  // добавляет новый обработчик Copier
  PipelineBuilder &CopyTo(string recipient);

  // добавляет новый обработчик Sender
  PipelineBuilder &Send(ostream &out);

  // возвращает готовую цепочку обработчиков
  unique_ptr<Worker> Build();

private:
  vector<Email> _letters;
  unique_ptr<Worker> _reader;
  Worker *_last = nullptr;
};

PipelineBuilder::PipelineBuilder(istream &is) {
  _last = new Reader(is);
  _reader = unique_ptr<Worker>(_last);
}

PipelineBuilder &PipelineBuilder::FilterBy(Filter::Function filter) {
  _last->SetNext(move(unique_ptr<Worker>(new Filter(filter))));
  return *this;
}

PipelineBuilder &PipelineBuilder::CopyTo(string recipient) {
  _last->SetNext(move(unique_ptr<Worker>(new Copier(recipient))));
  return *this;
}

PipelineBuilder &PipelineBuilder::Send(ostream &out) {
  _last->SetNext(move(unique_ptr<Worker>(new Sender(out))));
  return *this;
}

unique_ptr<Worker> PipelineBuilder::Build() { return move(_reader); }

void TestSanity() {
  string input = ("erich@example.com\n"
                  "richard@example.com\n"
                  "Hello there\n"

                  "erich@example.com\n"
                  "ralph@example.com\n"
                  "Are you sure you pressed the right button?\n"

                  "ralph@example.com\n"
                  "erich@example.com\n"
                  "I do not make mistakes of that kind\n");
  istringstream inStream(input);
  ostringstream outStream;

  PipelineBuilder builder(inStream);
  builder.FilterBy(
      [](const Email &email) { return email.from == "erich@example.com"; });
  builder.CopyTo("richard@example.com");
  builder.Send(outStream);
  auto pipeline = builder.Build();

  pipeline->Run();

  string expectedOutput = ("erich@example.com\n"
                           "richard@example.com\n"
                           "Hello there\n"

                           "erich@example.com\n"
                           "ralph@example.com\n"
                           "Are you sure you pressed the right button?\n"

                           "erich@example.com\n"
                           "richard@example.com\n"
                           "Are you sure you pressed the right button?\n");

  ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSanity);
  return 0;
}
