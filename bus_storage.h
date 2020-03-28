#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "id.h"

class BusStorage {
public:
  BusId Add(const std::string &bus_name);
  std::optional<BusId> TryFind(const std::string &bus_name) const;
  std::vector<std::string> GetBusNames(std::vector<BusId> bus_ids);
  std::string &GetName(BusId id) { return _buses[id]; }
  const std::string &GetName(BusId id) const { return _buses[id]; }

private:
  std::unordered_map<std::string, BusId> _bus_ids;
  std::vector<std::string> _buses;
};
