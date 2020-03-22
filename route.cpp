#include "route.h"

using namespace std;

Route::Route(bool circular, std::vector<string> stops)
    : _circular(circular), _stops(stops) {
  // TODO: calculate length and number of unique stops
}
