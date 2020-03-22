#include "route.h"

using namespace std;

Route::Route(bool circular, std::vector<string> stops)
    : _circular(circular), _stops(stops),
      _unique_stops(stops.begin(), stops.end()) {
  if (stops.size() < 2) {
    throw invalid_argument("There must be no less than 2 stops");
  }
}