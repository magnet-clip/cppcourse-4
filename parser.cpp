#include "parser.h"
#include "str_utils.h"

using namespace std;

CommandPtr Parser::ParseCommand(const string &command_type,
                                string_view line) const {
  if (command_type == Commands::StopCommand) {
    return make_shared<StopCommand>(ParseStopCommand(line));
  } else if (command_type == Commands::BusCommand) {
    return make_shared<BusCommand>(ParseBusCommand(line));
  }
  return nullptr;
}

QueryPtr Parser::ParseQuery(const string &query_type, string_view line) const {
  if (query_type == Queries::BusQuery) {
    return make_shared<BusQuery>(ParseBusQuery(line));
  } else if (query_type == Queries::StopQuery) {
    return make_shared<StopQuery>(ParseStopQuery(line));
  }
  return nullptr;
}

string ReadName(string_view &line) {
  RemoveLeadingSpaces(line);
  auto colon_pos = line.find(':');
  auto name = string(line.substr(0, colon_pos));
  line.remove_prefix(colon_pos + 1);
  RemoveLeadingSpaces(line);
  return name;
}

BusCommand StringParser::ParseBusCommand(string_view line) const {
  BusCommandBuilder builder(ReadName(line));

  auto delimiter_pos = line.find_first_of("->");
  if (delimiter_pos != string::npos) {
    builder.AddStop(RemoveTrailingSpaces(line.substr(0, delimiter_pos)));
    line.remove_prefix(delimiter_pos);

    bool circular = line[0] == '>';
    builder.SetIsCircular(circular);

    line.remove_prefix(1);
    char delimiter = circular ? '>' : '-';
    while (line.length() > 0) {
      RemoveLeadingSpaces(line);
      delimiter_pos = line.find(delimiter);
      if (delimiter_pos == string::npos) {
        builder.AddStop(RemoveTrailingSpaces(line));
        break;
      } else {
        builder.AddStop(RemoveTrailingSpaces(line.substr(0, delimiter_pos)));
        line.remove_prefix(delimiter_pos + 1);
      }
    }
  } else {
    // TODO: I suppose there should be no less than 2 stops in any route. Might
    // make sense to throw here
    builder.SetIsCircular(false);
    builder.AddStop(RemoveTrailingSpaces(line));
  }
  return builder.Build();
}

std::pair<std::string, double> ReadDistance(string_view distance_info) {
  RemoveLeadingSpaces(distance_info);
  // Read distance
  int m_pos = distance_info.find('m');
  double distance = stod(string(distance_info.substr(0, m_pos)));
  distance_info.remove_prefix(m_pos + 1);

  // Skip 'to'
  RemoveLeadingSpaces(distance_info);
  int space_pos = distance_info.find(' ');
  distance_info.remove_prefix(space_pos + 1);

  // Read stop name
  RemoveLeadingSpaces(distance_info);
  string name = string(distance_info);

  // Save
  return {name, distance};
}

StopCommand StringParser::ParseStopCommand(string_view line) const {
  StopCommandBuilder builder;
  // Read name
  int first_non_space = line.find_first_not_of(" \r\n\t");
  line.remove_prefix(first_non_space);
  int colon_pos = line.find(':');
  builder.SetName(line.substr(0, colon_pos));
  line.remove_prefix(colon_pos + 1);

  // Read location
  auto comma_pos = line.find(',');
  double latitude = stod(string(line.substr(0, comma_pos)));
  line.remove_prefix(comma_pos + 1);

  RemoveLeadingSpaces(line);
  comma_pos = line.find(' ');

  double longitude = stod(string(line.substr(0, comma_pos)));
  builder.SetLocation({Latitude(latitude), Longitude(longitude)});
  line.remove_prefix(comma_pos + 1);

  // Read distances
  if (comma_pos != string::npos) {
    comma_pos = line.find(',');
    do {
      if (comma_pos == string::npos) {
        const auto &distance = ReadDistance(line);
        builder.AddDistance(distance.first, distance.second);
        break;
      } else {
        const auto &distance = ReadDistance(line.substr(0, comma_pos));
        builder.AddDistance(distance.first, distance.second);
      }
      line.remove_prefix(comma_pos + 1);
      comma_pos = line.find(',');
    } while (true);
  }

  return builder.Build();
}

BusQuery StringParser::ParseBusQuery(string_view line) const {
  RemoveLeadingSpaces(line);
  return BusQuery(string(line));
}

StopQuery StringParser::ParseStopQuery(string_view line) const {
  RemoveLeadingSpaces(line);
  return StopQuery(string(line));
}

BusCommand JsonParser::ParseBusCommand(string_view line) const {}
StopCommand JsonParser::ParseStopCommand(string_view line) const {}
BusQuery JsonParser::ParseBusQuery(string_view line) const {}
StopQuery JsonParser::ParseStopQuery(string_view line) const {}