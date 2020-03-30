#pragma once

#include "command.h"
#include "database.h"
#include "parser.h"
#include "serializer.h"

#include <iostream>

class Io {
public:
  Io(const Serializer &serializer) : _serializer(serializer) {}
  virtual CommandPtr ReadCommand(std::istream &is) = 0;
  virtual QueryPtr ReadQuery(std::istream &is) = 0;

  virtual std::vector<CommandPtr> ReadCommands(std::istream &is) = 0;
  virtual std::vector<QueryPtr> ReadQueries(std::istream &is) = 0;

  virtual void InAndOut(std::istream &is, std::ostream &os) = 0;

  std::string Serialize(ResponsePtr response) const {
    return _serializer.Serialize(response);
  }

protected:
  const Serializer &_serializer;
};

class StringIo : public Io {
public:
  StringIo() : Io(StringSerializer()) {}
  virtual CommandPtr ReadCommand(std::istream &is) override;
  virtual QueryPtr ReadQuery(std::istream &is) override;

  virtual std::vector<CommandPtr> ReadCommands(std::istream &is) override;
  virtual std::vector<QueryPtr> ReadQueries(std::istream &is) override;
  virtual void InAndOut(std::istream &is, std::ostream &os) override;

private:
  StringParser _parser;
};

class JsonIo : public Io {
public:
  JsonIo() : Io(JsonSerializer()) {}
  virtual CommandPtr ReadCommand(std::istream &is) override;
  virtual QueryPtr ReadQuery(std::istream &is) override;

  virtual std::vector<CommandPtr> ReadCommands(std::istream &is) override;
  virtual std::vector<QueryPtr> ReadQueries(std::istream &is) override;
  virtual void InAndOut(std::istream &is, std::ostream &os) override;

private:
  JsonParser _parser;
};
