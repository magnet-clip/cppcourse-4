#pragma once

#include "bus_storage.h"
#include "map_storage.h"
#include "stop_storage.h"

#include <string>

class MapStorageSerializer {
public:
  MapStorageSerializer(const BusStorage &bus, const StopStorage &stop) : _bus(bus), _stop(stop) {}
  std::string SerializeToString(const MapStorage &storage) const;
  std::string SerializeToDot(const MapStorage &storage) const;

private:
  const BusStorage &_bus;
  const StopStorage &_stop;
};