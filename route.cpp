#include "route.h"

using namespace std;

Route::Route(const Planet &planet, bool circular, std::vector<Stop> stops)
    : _planet(planet), _circular(circular), _stops(stops) {
  // TODO: calculate length and number of unique stops
}
