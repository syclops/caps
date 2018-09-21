//
// Created by smaptas on 25.10.17.
//

#ifndef CAPS_CONTAINS_H
#define CAPS_CONTAINS_H

template <typename MapType, typename KeyType>
inline bool contains(const MapType& map, const KeyType& key)
{
  return map.find(key) != map.end();
}

#endif //CAPS_CONTAINS_H
