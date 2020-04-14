#include "route.h"

using namespace std;

Route::Route(bool circular, vector<StopId> stops)
    : _stops(stops), _unique_stops(stops.begin(), stops.end()), _circular(circular) {

  if (stops.size() < 2) {
    throw invalid_argument("There must be no less than 2 stops");
  }

  if (!circular) {
    _stops.insert(_stops.end(), next(stops.rbegin()), stops.rend());
  }
}