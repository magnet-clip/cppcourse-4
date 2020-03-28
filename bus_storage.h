#pragma once

#include <memory>
#include <string>
#include <vector>

#include "id.h"

struct Bus {
  BusId id;
  std::string name;
};

using BusPtr = std::shared_ptr<Bus>;

class BusStorage {
public:
  BusId Add(const std::string &bus_name);
  Bus &Get(BusId id) { return _buses[id]; }
  const Bus &Get(BusId id) const { return _buses[id]; }

private:
  static BusStorage _instance;
  std::vector<Bus> _buses;
};
