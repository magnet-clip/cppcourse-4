#include "serializer.h"

#include "json.h"

#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

string Serializer::Serialize(ResponsePtr response) const {
  if (response->Kind() == Responses::FoundBusResponse) {
    return Serialize(static_cast<const FoundBusResponse &>(*response));
  } else if (response->Kind() == Responses::NoBusResponse) {
    return Serialize(static_cast<const NoBusResponse &>(*response));
  } else if (response->Kind() == Responses::FoundStopResponse) {
    return Serialize(static_cast<const FoundStopResponse &>(*response));
  } else if (response->Kind() == Responses::NoStopResponse) {
    return Serialize(static_cast<const NoStopResponse &>(*response));
  } else {
    throw domain_error(response->Kind());
  }
}

string StringSerializer::Serialize(const NoBusResponse &response) const {
  ostringstream os;
  os << "Bus " << response.bus_name << ": not found";
  return os.str();
}

string StringSerializer::Serialize(const FoundBusResponse &response) const {
  ostringstream os;
  os.precision(7);
  os << "Bus " << response.bus_name << ": " << response.num_stops
     << " stops on route, " << response.num_unique_stops << " unique stops, "
     << response.length << " route length, " << response.curvature
     << " curvature";
  return os.str();
}

string StringSerializer::Serialize(const NoStopResponse &response) const {
  ostringstream os;
  os << "Stop " << response.stop_name << ": not found";
  return os.str();
}

string StringSerializer::Serialize(const FoundStopResponse &response) const {
  ostringstream os;
  os << "Stop " << response.stop_name << ": ";
  if (response.bus_names.size() == 0) {
    os << "no buses";
  } else {
    os << "buses";
    for (const auto &bus_name : response.bus_names) {
      os << " " << bus_name;
    }
  }
  return os.str();
}

string JsonSerializer::Serialize(const NoBusResponse &response) const {
  using Json::Node;
  map<string, Node> res;
  res.insert({"request_id", {response.GetId()}});
  res.insert({"error_message", {"not found"s}});
  Node result{res};

  return result.ToString();
}

string JsonSerializer::Serialize(const FoundBusResponse &response) const {
  using Json::Node;
  map<string, Node> res;
  res.insert({"request_id", {response.GetId()}});
  res.insert({"route_length", {response.length}});
  res.insert({"curvature", {response.curvature}});
  res.insert({"stop_count", {response.num_stops}});
  res.insert({"unique_stop_count", {response.num_unique_stops}});
  Node result{res};

  return result.ToString();
}

string JsonSerializer::Serialize(const NoStopResponse &response) const {
  using Json::Node;
  map<string, Node> res;
  res.insert({"request_id", {response.GetId()}});
  res.insert({"error_message", {"not found"s}});
  Node result{res};

  return result.ToString();
}

string JsonSerializer::Serialize(const FoundStopResponse &response) const {
  using Json::Node;
  map<string, Node> res;
  res.insert({"request_id", {response.GetId()}});
  vector<Node> buses;
  for (const auto &bus_name : response.bus_names) {
    buses.push_back({bus_name});
  }
  res.insert({"buses", {buses}});
  Node result{res};

  return result.ToString();
}
