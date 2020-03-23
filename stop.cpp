#include "stop.h"
#include <tuple>
using namespace std;

bool operator==(const Stop &a, const Stop &b) {
  return a.GetName() == b.GetName();
}

bool operator==(const StopPair &a, const StopPair &b) {
  return tie(a.GetFirst(), a.GetSecond()) == tie(b.GetFirst(), b.GetSecond());
}
