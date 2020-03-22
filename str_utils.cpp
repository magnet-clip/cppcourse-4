#include "str_utils.h"

using namespace std;

void RemoveLeadingSpaces(string_view &line) {
  int first_non_space = line.find_first_not_of(' ');
  line.remove_prefix(first_non_space);
}

string_view RemoveTrailingSpaces(string_view line) {
  int first_non_space = line.find_last_not_of(' ');
  line.remove_suffix(line.size() - (first_non_space + 1));
  return line;
}