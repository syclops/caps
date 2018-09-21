/**
 * CAPS common utility functions
 * Author: Steve Matsumoto
 */

template <typename SetType>
SetType add_sets(const SetType& set1, const SetType& set2)
{
  SetType combined_set;
  if (set1.size() < set2.size()) {
    combined_set = set2;
    combined_set.insert(set1.begin(), set1.end());
  } else {
    combined_set = set1;
    combined_set.insert(set2.begin(), set2.end());
  }
  return combined_set;
}