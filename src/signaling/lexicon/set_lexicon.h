/**
 * Author: smaptas
 */

#ifndef CAPS_SETLEXICON_H
#define CAPS_SETLEXICON_H

// Include C standard libraries.

// Include C++ standard libraries.

// Include other headers from this project.
#include "lexicon.h"

// Include headers from other projects.

class SetLexicon: public Lexicon
{
 public:

  SetLexicon();

  ~SetLexicon() override = default;

  void add_string(const std::string& str) override;

  bool has_string(const std::string& str) const override;

  int size() const override;

  void load(std::istream& instream) override;

  void dump(std::ostream& outstream) const override;

  std::set<std::string> dump_strings() const override;

 private:
  std::set<std::string> set_;
};

#endif //CAPS_SETLEXICON_H
