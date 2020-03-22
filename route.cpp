#include "route.h"

using namespace std;

Route::Route(bool circular, std::vector<string> stops)
    : _stop_names(stops), _unique_stops(stops.begin(), stops.end()) {

  if (stops.size() < 2) {
    throw invalid_argument("There must be no less than 2 stops");
  }

  if (!circular) {
    _stop_names.insert(_stop_names.end(), next(stops.rbegin()), stops.rend());
  }
}