#include "parser.h"
#include "json.h"
#include "str_utils.h"

using namespace std;

CommandPtr Parser::ParseCommand(const string &command_type) {
  if (command_type == Commands::StopCommand) {
    return make_shared<StopCommand>(ParseStopCommand());
  } else if (command_type == Commands::BusCommand) {
    return make_shared<BusCommand>(ParseBusCommand());
  }
  return nullptr;
}

QueryPtr Parser::ParseQuery(const string &query_type) {
  if (query_type == Queries::BusQuery) {
    return make_shared<BusQuery>(ParseBusQuery());
  } else if (query_type == Queries::StopQuery) {
    return make_shared<StopQuery>(ParseStopQuery());
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

BusCommand StringParser::ParseBusCommand() {
  BusCommandBuilder builder(ReadName(_line));

  auto delimiter_pos = _line.find_first_of("->");
  if (delimiter_pos != string::npos) {
    builder.AddStop(RemoveTrailingSpaces(_line.substr(0, delimiter_pos)));
    _line.remove_prefix(delimiter_pos);

    bool circular = _line[0] == '>';
    builder.SetIsCircular(circular);

    _line.remove_prefix(1);
    char delimiter = circular ? '>' : '-';
    while (_line.length() > 0) {
      RemoveLeadingSpaces(_line);
      delimiter_pos = _line.find(delimiter);
      if (delimiter_pos == string::npos) {
        builder.AddStop(RemoveTrailingSpaces(_line));
        break;
      } else {
        builder.AddStop(RemoveTrailingSpaces(_line.substr(0, delimiter_pos)));
        _line.remove_prefix(delimiter_pos + 1);
      }
    }
  } else {
    // TODO: I suppose there should be no less than 2 stops in any route. Might
    // make sense to throw an exception here
    builder.SetIsCircular(false);
    builder.AddStop(RemoveTrailingSpaces(_line));
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

StopCommand StringParser::ParseStopCommand() {
  StopCommandBuilder builder;
  // Read name
  int first_non_space = _line.find_first_not_of(" \r\n\t");
  _line.remove_prefix(first_non_space);
  int colon_pos = _line.find(':');
  builder.SetName(_line.substr(0, colon_pos));
  _line.remove_prefix(colon_pos + 1);

  // Read location
  auto comma_pos = _line.find(',');
  double latitude = stod(string(_line.substr(0, comma_pos)));
  _line.remove_prefix(comma_pos + 1);

  RemoveLeadingSpaces(_line);
  comma_pos = _line.find(' ');

  double longitude = stod(string(_line.substr(0, comma_pos)));
  builder.SetLocation({Latitude(latitude), Longitude(longitude)});
  _line.remove_prefix(comma_pos + 1);

  // Read distances
  if (comma_pos != string::npos) {
    comma_pos = _line.find(',');
    do {
      if (comma_pos == string::npos) {
        const auto &distance = ReadDistance(_line);
        builder.AddDistance(distance.first, distance.second);
        break;
      } else {
        const auto &distance = ReadDistance(_line.substr(0, comma_pos));
        builder.AddDistance(distance.first, distance.second);
      }
      _line.remove_prefix(comma_pos + 1);
      comma_pos = _line.find(',');
    } while (true);
  }

  return builder.Build();
}

BusQuery StringParser::ParseBusQuery() {
  RemoveLeadingSpaces(_line);
  return BusQuery(string(_line));
}

StopQuery StringParser::ParseStopQuery() {
  RemoveLeadingSpaces(_line);
  return StopQuery(string(_line));
}

BusCommand JsonParser::ParseBusCommand() {
  auto data = _node.AsMap();

  BusCommandBuilder builder(data.at("name").AsString());

  builder.SetIsCircular(data.at("is_roundtrip").AsBool());
  if (data.count("stops")) {
    auto stops = data.at("stops").AsArray();
    for (const auto &name : stops) {
      builder.AddStop(name.AsString());
    }
  }

  return builder.Build();
}

StopCommand JsonParser::ParseStopCommand() {
  auto data = _node.AsMap();

  StopCommandBuilder builder;

  builder.SetName(data.at("name").AsString());

  double latitude = data.at("latitude").AsDouble();
  double longitude = data.at("longitude").AsDouble();
  builder.SetLocation({Latitude(latitude), Longitude(longitude)});

  if (data.count("road_distances")) {
    auto distances = data.at("road_distances").AsMap();
    for (const auto &[name, node] : distances) {
      builder.AddDistance(name, node.AsInt());
    }
  }

  return builder.Build();
}

BusQuery JsonParser::ParseBusQuery() {
  auto data = _node.AsMap();
  auto name = data.at("name").AsString();
  auto id = static_cast<size_t>(data.at("id").AsInt());
  return {name, id};
}

StopQuery JsonParser::ParseStopQuery() {
  auto data = _node.AsMap();
  auto name = data.at("name").AsString();
  auto id = static_cast<size_t>(data.at("id").AsInt());
  return {name, id};
}