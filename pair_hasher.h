#pragma once

#include <utility>

struct PairHasher {
  std::hash<size_t> vtx_hash;
  size_t operator()(const std::pair<size_t, size_t> &p) const {
    size_t res = 17;
    res = 31 * res + vtx_hash(p.first);
    res = 31 * res + vtx_hash(p.second);
    return res;
  }
};