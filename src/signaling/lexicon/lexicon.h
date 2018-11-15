/**
 * Author: smaptas
 */

#ifndef CAPS_LEXICON_H
#define CAPS_LEXICON_H

// Include C standard libraries.

// Include C++ standard libraries.
#include <iostream>
#include <set>
#include <string>
#include <unordered_set>

// Include other headers from this project.

// Include headers from other projects.

class Lexicon
{
 public:

  Lexicon();

  virtual ~Lexicon();

  virtual void add_file(std::istream& instream);

  virtual void add_string(const std::string& str) = 0;

  virtual bool has_string(const std::string& str) const = 0;

  virtual int size() const;

  virtual void load(std::istream& instream) = 0;

  virtual void dump(std::ostream& outstream) const = 0;

  virtual std::set<std::string> dump_strings() const = 0;

 protected:

  int size_;
};

bool operator==(const Lexicon& lhs, const Lexicon& rhs);

bool operator!=(const Lexicon& lhs, const Lexicon& rhs);

#endif //CAPS_LEXICON_H
