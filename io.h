#pragma once

#include "command.h"
#include "database.h"
#include "parser.h"
#include "serializer.h"

#include <iostream>

using CommandsAndQueries =
    std::pair<std::vector<CommandPtr>, std::vector<QueryPtr>>;

class Io {
public:
  Io(SerializerPtr serializer) : _serializer(serializer) {}

  virtual CommandPtr ReadCommand(std::istream &is) = 0;
  virtual QueryPtr ReadQuery(std::istream &is) = 0;
  virtual CommandsAndQueries ReadCommandsAndQueries(std::istream &is) = 0;

  std::vector<std::string>
  ProcessResponses(const std::vector<ResponsePtr> &responses) const {
    std::vector<std::string> res;
    for (const auto &response : responses) {
      res.push_back(_serializer->Serialize(response));
    }
    return res;
  };

private:
  SerializerPtr _serializer;
};

class StringIo : public Io {
public:
  StringIo() : Io(std::make_shared<StringSerializer>()) {}
  virtual CommandPtr ReadCommand(std::istream &is) override;
  virtual QueryPtr ReadQuery(std::istream &is) override;

  virtual CommandsAndQueries ReadCommandsAndQueries(std::istream &is) override;

  std::vector<CommandPtr> ReadCommands(std::istream &is);
  std::vector<QueryPtr> ReadQueries(std::istream &is);

private:
  StringParser _parser;
};

class JsonIo : public Io {
public:
  JsonIo() : Io(std::make_shared<JsonSerializer>()) {}

  virtual CommandPtr ReadCommand(std::istream &is) override;
  virtual QueryPtr ReadQuery(std::istream &is) override;

  virtual CommandsAndQueries ReadCommandsAndQueries(std::istream &is) override;

private:
  JsonParser _parser;
};

void InAndOut(std::istream &is, std::ostream &os, Io &io);
