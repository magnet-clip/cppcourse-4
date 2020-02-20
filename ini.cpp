#include "ini.h"
#include "test_runner.h"
#include <iostream>
using namespace std;

namespace Ini {
Document Load(istream &input) {
  Document result;
  Section *current_section;
  for (string str; getline(input, str);) {
    string_view view = str;
    if (view.find_first_not_of('\n') == string::npos) {
      continue;
    } else if (view[0] == '[') {
      view.remove_prefix(1);
      auto closing_bracket_pos = view.find(']');
      string section_name{view.substr(0, closing_bracket_pos)};
      //   cerr << "[" << section_name << "]" << endl;
      current_section = &result.AddSection(section_name);
    } else {
      auto equal_sign_pos = view.find('=');
      string var_name{view.substr(0, equal_sign_pos)};
      view.remove_prefix(equal_sign_pos + 1);
      string var_value{view};
      //   cerr << var_name << "=" << var_value << endl;
      current_section->insert({var_name, var_value});
    }
    // cerr << *current_section << endl;
  }
  return result;
}

} // namespace Ini