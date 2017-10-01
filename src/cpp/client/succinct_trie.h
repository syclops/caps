#ifndef CAPS_SUCCINCT_TRIE_H_
#define CAPS_SUCCINCT_TRIE_H_

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

#endif  // CAPS_SUCCINCT_TRIE_H_
