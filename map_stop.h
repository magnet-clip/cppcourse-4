#pragma once

#include "id.h"

#include <memory>

class MapStop {
public:
  MapStop(VertexId id, StopId stop_id) : _id(id), _stop_id(stop_id) {}
  VertexId GetId() const { return _id; }
  StopId GetStopId() const { return _stop_id; }
  virtual bool IsWait() const = 0;

private:
  VertexId _id;
  StopId _stop_id;
};

class WaitStop final : public MapStop {
public:
  using MapStop::MapStop;
  virtual bool IsWait() const override { return true; };
};

using MapStopPtr = std::shared_ptr<MapStop>;

class BusStop final : public MapStop {
public:
  BusStop(VertexId id, BusId bus_id, StopId stop_id)
      : MapStop(id, stop_id), _bus_id(bus_id) {}
  StopId GetBusId() const { return _bus_id; }
  virtual bool IsWait() const override { return false; };

private:
  BusId _bus_id;
};