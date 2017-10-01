#ifndef CAPS_ENTROPY_H
#define CAPS_ENTROPY_H

#include <cstdlib>
#include <map>
#include <unordered_map>
#include <vector>

/**
 * Calculate the entropy of the values of a map, where the values are
 * probabilities of each of the keys.
 * @tparam K the type of key used for the map.
 * @param map the map of keys to probabilities.
 * @return the entropy of the map values.
 */
template<typename K>
double entropy(const std::map<K, double>& map)
{
  double value = 0;
  for (auto it: map) {
    value -= it.second * log2(it.second);
  }
  return value;
}

template<typename K>
double entropy(const std::map<K, size_t>& map)
{
  size_t total = 0;
  for (auto it: map) {
    total += it.second;
  }
  std::map<K, double> normalized_map;
  for (auto it: map) {
    normalized_map[it.first] = it.second / (double) total;
  }
  return entropy(normalized_map);
}

template<typename K>
double entropy(std::unordered_map<K, double>& map)
{
  double value = 0;
  for (auto it: map) {
    value -= it.second * log2(it.second);
  }
  return value;
}

template<typename K>
double entropy(std::unordered_map<K, size_t>& map)
{
  size_t total = 0;
  for (auto it: map) {
    total += it.second;
  }
  std::map<K, double> double_map;
  for (auto it: map) {
    double_map[it.first] = it.second / (double) total;
  }
  return entropy(double_map);
}

double entropy(std::vector<double>& values)
{
  double value = 0;
  for (double d: values) {
    value -= d * log2(d);
  }
  return value;
}

double entropy(std::vector<size_t>& values)
{
  size_t total = 0;
  for (size_t v: values) {
    total += v;
  }
  std::vector<double> normalized_values;
  for (size_t v: values) {
    normalized_values.push_back(v / (double) total);
  }
  return entropy(normalized_values);
}

#endif //CAPS_ENTROPY_H
