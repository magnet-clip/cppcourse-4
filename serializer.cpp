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

string StringSerializer::Serialize(const NoRouteResponse &response) const {
  throw domain_error("Routes not serializable");
}

string StringSerializer::Serialize(const FoundRouteResponse &response) const {
  throw domain_error("Routes not serializable");
}

Json::Node GetNotFoundNode(int id) {
  map<string, Json::Node> res;
  res.insert({"request_id", {id}});
  res.insert({"error_message", {"not found"s}});
  return {res};
}

Json::Node GetJsonNode(const NoBusResponse &response) {
  return GetNotFoundNode(response.GetId());
}

Json::Node GetJsonNode(const FoundBusResponse &response) {
  map<string, Json::Node> res;
  res.insert({"request_id", {response.GetId()}});
  res.insert({"route_length", {response.length}});
  res.insert({"curvature", {response.curvature}});
  res.insert({"stop_count", {response.num_stops}});
  res.insert({"unique_stop_count", {response.num_unique_stops}});
  return {res};
}

Json::Node GetJsonNode(const NoStopResponse &response) {
  return GetNotFoundNode(response.GetId());
}

Json::Node GetJsonNode(const FoundStopResponse &response) {
  map<string, Json::Node> res;
  res.insert({"request_id", {response.GetId()}});
  vector<Json::Node> buses;
  for (const auto &bus_name : response.bus_names) {
    buses.push_back({bus_name});
  }
  res.insert({"buses", {buses}});
  return {res};
}

Json::Node GetJsonNode(const NoRouteResponse &response) {
  return GetNotFoundNode(response.GetId());
}
Json::Node GetJsonNode(const BusRouteItem &item) {
  map<string, Json::Node> res;
  res.insert({"bus", {item.bus_name}});
  res.insert({"span_count", {item.span_count}});
  res.insert({"time", {item.time}});
  res.insert({"type", {item.Kind()}});
  return res;
}
Json::Node GetJsonNode(const WaitRouteItem &item) {
  map<string, Json::Node> res;
  res.insert({"stop_name", {item.stop_name}});
  res.insert({"time", {item.time}});
  res.insert({"type", {item.Kind()}});
  return res;
}
Json::Node GetJsonNode(const FoundRouteResponse &response) {
  map<string, Json::Node> res;
  res.insert({"request_id", {response.GetId()}});
  res.insert({"total_time", {response.total_time}});
  vector<Json::Node> items;
  for (const auto &item : response.items) {
    if (item->Kind() == RouteItemType::Bus) {
      items.push_back(GetJsonNode(static_cast<BusRouteItem &>(*item)));
    } else if (item->Kind() == RouteItemType::Wait) {
      items.push_back(GetJsonNode(static_cast<WaitRouteItem &>(*item)));
    } else {
      throw domain_error("Unknown item type");
    }
  }
  res.insert({"items", {items}});
  return res;
}