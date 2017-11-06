//
// Created by smaptas on 26.10.17.
//

#ifndef CAPS_POWERSET_H
#define CAPS_POWERSET_H

#include <set>
#include <vector>

template <typename T>
std::set<std::set<T>> powerset(const std::set<T>& orig_set)
{
  std::set<std::set<T>> powerset;
  powerset.emplace();
  for (auto value: orig_set) {
    std::vector<std::set<T>> add_sets(powerset.size());
    for (auto subset: powerset) {
      auto new_subset{subset};
      new_subset.emplace(value);
      add_sets.push_back(new_subset);
    }
    for (auto add_set: add_sets) {
      powerset.insert(add_set);
    }
  }
  return powerset;
}

#endif //CAPS_POWERSET_H
