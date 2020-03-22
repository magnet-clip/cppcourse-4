#pragma once

#include "geomath.h"
#include <vector>

class SpanIteratorProvider {
public:
  SpanIteratorProvider(const std::vector<std::string> &stops, bool circular) {
    _stops.assign(stops.begin(), stops.end());
    if (!circular) {
      _stops.insert(_stops.end(), next(stops.rbegin()), stops.rend());
    }
  }

  std::vector<std::string>::iterator begin() { return _stops.begin(); };
  std::vector<std::string>::iterator end() { return _stops.end(); };
  std::vector<std::string>::const_iterator begin() const {
    return _stops.begin();
  };
  std::vector<std::string>::const_iterator end() const { return _stops.end(); };

private:
  std::vector<std::string> _stops;
};

class Route {
public:
  Route(bool circular, std::vector<std::string> stops);

  SpanIteratorProvider GetStopsIterator() const {
    return SpanIteratorProvider(_stops, _circular);
  }

  int StopsCount() const {
    return _circular ? _stops.size() : 2 * _stops.size() - 1;
  }

  int UniqueStopsCount() const { return _unique_stops_count; }

private:
  bool _circular;
  int _unique_stops_count = 0;
  std::vector<std::string> _stops;
};
