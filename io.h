#pragma once

#include "command.h"
#include "database.h"
#include "parser.h"
#include "serializer.h"

#include <iostream>

CommandPtr ReadCommand(std::istream &is, const Parser &parser);
QueryPtr ReadQuery(std::istream &is, const Parser &parser);

std::vector<CommandPtr> ReadCommands(std::istream &is, const Parser &parser);
std::vector<QueryPtr> ReadQueries(std::istream &is, const Parser &parser);

void InAndOut(std::istream &is, std::ostream &os, const Serializer &serializer,
              const Parser &parser);