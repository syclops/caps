//
// Created by smaptas on 27.10.17.
//

#include "fsa_lexicon.h"

int main() {
  FSALexicon lexicon;
  lexicon.add_string("abdcb");
  lexicon.add_string("abfhgb");
  lexicon.add_string("acgb");
  lexicon.add_string("bac");
  lexicon.add_string("baeb");
  lexicon.add_string("bdcb");
  lexicon.finalize();
  lexicon.compact();
  return 0;
}
