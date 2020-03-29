#pragma once

#include "command.h"
#include "database.h"
#include "serializer.h"

#include <iostream>

void InAndOut(std::istream &is, std::ostream &os,
              const Serializer &serializer) {
  auto commands = ReadCommands(is);
  auto queries = ReadQueries(is);

  Database db;
  db.ExecuteCommands(commands);
  auto res = db.ExecuteQueries(queries);
  for (const auto &out : res) {
    os << serializer.Serialize(out) << std::endl;
  }
}