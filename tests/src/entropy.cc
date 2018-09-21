#include "entropy.h"

#include <cmath>
#include <cstdlib>
#include <map>
#include <unordered_map>
#include <vector>

template<typename K>
double entropy(const std::map<K, double>& map)
{
  double entropy = 0;
  for (auto it: map) {
    entropy -= it.second * log2(it.second);
  }
  return entropy;
}

