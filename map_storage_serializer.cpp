#include "map_storage_serializer.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

std::string MapStorageSerializer::SerializeToDot(const MapStorage &storage) const {
  vector<string> palette{"#f2f3ae", "#edd382", "#fc9e4f", "#f4442e", "#020122", "#f2cbce", "#eba5a8", "#f77174", "#eb3738", "#011312"};
  stringstream ss;
  unordered_map<BusId, unordered_map<StopId, VertexIdSet>>
      by_bus_and_stop;

  for (const auto &[stop_id, bus_and_vertices] : storage.GetVerticesByBusStops()) {
    for (const auto &[bus_id, vertices] : bus_and_vertices) {
      auto bus_it = by_bus_and_stop.find(bus_id);
      if (bus_it == by_bus_and_stop.end()) {
        auto insert_result = by_bus_and_stop.insert({bus_id, {}});
        bus_it = insert_result.first;
      }

      auto &by_stop = bus_it->second;
      auto stop_it = by_stop.find(stop_id);
      if (stop_it == by_stop.end()) {
        auto insert_result = by_stop.insert({stop_id, {}});
        stop_it = insert_result.first;
      }

      stop_it->second.insert(vertices.begin(), vertices.end());
    }
  }

  ss << "digraph main {" << endl;
  ss << "/* WAIT stops */" << endl;
  for (const auto &[stop_id, vertex_id] : storage.GetVerticesByWaitStops()) {
    ss << vertex_id << " [label=\"" << _stop.Get(stop_id)->GetName()
       << "\", shape=hexagon];" << endl;
  }
  ss << "/* BUS stops */" << endl;
  for (const auto &[bus_id, stops_and_vertices] : by_bus_and_stop) {
    for (const auto &[stop_id, vertices] : stops_and_vertices) {
      for (const auto vertex_id : vertices) {
        ss << vertex_id << " [label=\"" << _bus.GetName(bus_id) << " / " << _stop.Get(stop_id)->GetName()
           << "\", shape=ellipse, style=filled, color=\"" << (palette[bus_id % palette.size()]) << "\"];" << endl;
      }
    }
  }
  ss << "/* ROUTES */" << endl;
  const auto &incidents = storage.GetIncidents();
  for (VertexId from = 0; from < incidents.size(); from++) {
    for (const auto &info : incidents[from]) {
      ss << from << " -> " << info.vertex_id << " [len=" << info.edge_weight << "];" << endl;
    }
  }
  ss << "}" << endl;
  return ss.str();
}

string MapStorageSerializer::SerializeToString(const MapStorage &storage) const {
  stringstream ss;
  unordered_map<BusId, unordered_map<StopId, VertexIdSet>>
      by_bus_and_stop;

  for (const auto &[stop_id, bus_and_vertices] : storage.GetVerticesByBusStops()) {
    for (const auto &[bus_id, vertices] : bus_and_vertices) {
      auto bus_it = by_bus_and_stop.find(bus_id);
      if (bus_it == by_bus_and_stop.end()) {
        auto insert_result = by_bus_and_stop.insert({bus_id, {}});
        bus_it = insert_result.first;
      }

      auto &by_stop = bus_it->second;
      auto stop_it = by_stop.find(stop_id);
      if (stop_it == by_stop.end()) {
        auto insert_result = by_stop.insert({stop_id, {}});
        stop_it = insert_result.first;
      }

      stop_it->second.insert(vertices.begin(), vertices.end());
    }
  }

  ss << "Wait Stops" << endl;
  ss << setw(10) << "Stop ID"
     << setw(30) << "Stop Name"
     << setw(10) << "Vertex ID" << endl;

  for (const auto &[stop_id, vertex_id] : storage.GetVerticesByWaitStops()) {
    ss << setw(10) << stop_id
       << setw(30) << _stop.Get(stop_id)->GetName()
       << setw(10) << vertex_id << endl;
  }
  ss << "Bus Stops" << endl;
  ss << setw(10) << "Bus ID"
     << setw(10) << "Bus Name"
     << setw(10) << "Stop ID"
     << setw(30) << "Stop Name"
     << setw(10) << "Vertex ID" << endl;
  for (const auto &[bus_id, stops_and_vertices] : by_bus_and_stop) {
    for (const auto &[stop_id, vertices] : stops_and_vertices) {
      for (const auto vertex_id : vertices) {
        ss << setw(10) << bus_id
           << setw(10) << _bus.GetName(bus_id)
           << setw(10) << stop_id
           << setw(30) << _stop.Get(stop_id)->GetName()
           << setw(10) << vertex_id << endl;
      }
    }
  }
  ss << "Routes" << endl;
  ss << setw(10) << "Edge Id"
     << setw(15) << "From Vertex ID"
     << setw(15) << "To Vertex Id"
     << setw(10) << "Weight" << endl;

  size_t idx = 0;
  const auto &incidents = storage.GetIncidents();
  for (VertexId from = 0; from < incidents.size(); from++) {
    for (const auto &info : incidents[from]) {
      ss << setw(10) << idx++
         << setw(15) << from
         << setw(15) << info.vertex_id
         << setw(10) << info.edge_weight << endl;
    }
  }
  return ss.str();
}