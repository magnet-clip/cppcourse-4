#pragma once

#include <istream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace Ini {
using Section = std::unordered_map<std::string, std::string>;

class Document {
public:
  Section &AddSection(std::string name) { return sections[name]; }
  const Section &GetSection(const std::string &name) const {
    auto it = sections.find(name);
    if (it == sections.end()) {
      throw std::out_of_range("Unknown section [" + name + "");
    }
    return it->second;
  }
  size_t SectionCount() const { return sections.size(); }

private:
  std::unordered_map<std::string, Section> sections;
};

Document Load(std::istream &input);
} // namespace Ini