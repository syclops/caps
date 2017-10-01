#ifndef CAPS_TRIE_H_
#define CAPS_TRIE_H_

#include <iostream>
#include <string>
#include "graphbase.h"
#include "node.h"

class Trie: public GraphBase
{
 public:

  Trie() = default;

  // Rule of five methods.
  Trie(const Trie& orig) = default;
  Trie(Trie&& orig) noexcept;
  Trie& operator=(Trie rhs);
  ~Trie() override = default;
  friend void swap(Trie& first, Trie& second);

  void dump_bin(std::ostream& out_stream) const override;

  void load_bin(std::istream& in_stream) override;

 protected:

  void add_string(std::string str) override;
};

#endif  // CAPS_TRIE_H_
