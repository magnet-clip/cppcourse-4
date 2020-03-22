#include "route.h"
#include <unordered_set>

using namespace std;

Route::Route(bool circular, std::vector<string> stops)
    : _circular(circular), _stops(stops) {
  if (stops.size() < 2) {
    throw invalid_argument("There must be no less than 2 stops");
  }

  unordered_set<string> unique_stops(stops.begin(), stops.end());
  _unique_stops_count = unique_stops.size();
}