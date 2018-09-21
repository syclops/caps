#ifndef CAPS_GRAPH_H
#define CAPS_GRAPH_H

#include "succinct_base.h"

class SuccinctTrie: public SuccinctGraphBase
{
 public:

  SuccinctTrie() = default;
  SuccinctTrie(SuccinctTrie& orig) = default;
  SuccinctTrie(SuccinctTrie&& orig) = default;
  SuccinctTrie& operator=(SuccinctTrie rhs) = default;
  ~SuccinctTrie() = default;

  void load_file(std::string filename);
  bool in_set(std::string str);
};

#endif  // CAPS_GRAPH_H
