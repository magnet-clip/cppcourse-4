#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "id.h"

class BusIdIterator {
public:
  BusIdIterator(int num) : _num(num) {}
  operator int() { return _num; }
  int operator*() { return _num; }
  BusIdIterator &operator++() {
    _num++;
    return *this;
  }

private:
  int _num;
};

class BusIdRange {
public:
  explicit BusIdRange(int max_id) : _max_id(max_id) {}
  BusIdIterator begin() const { return 0; }
  BusIdIterator end() const { return _max_id; }

private:
  int _max_id;
};

class BusStorage {
public:
  BusId Add(const std::string &bus_name);
  BusIdRange GetBuses() const {
    return BusIdRange(_buses.size());
  }
  std::optional<BusId> TryFind(const std::string &bus_name) const;
  std::vector<std::string> GetBusNames(std::vector<BusId> bus_ids);
  std::string &GetName(BusId id) { return _buses[id]; }
  const std::string &GetName(BusId id) const { return _buses[id]; }

private:
  std::unordered_map<std::string, BusId> _bus_ids;
  std::vector<std::string> _buses;
};
