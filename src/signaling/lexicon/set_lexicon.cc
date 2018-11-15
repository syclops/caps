/**
 * Author: smaptas
 */

// Include header file.
#include "set_lexicon.h"

// Include C standard libraries.

// Include C++ standard libraries.
#include <iostream>
#include <sstream>

// Include other headers from this project.

// Include header from other projects.

SetLexicon::SetLexicon()
  : set_()
{
  // Nothing to do here.
}

void SetLexicon::add_string(const std::string& str)
{
  set_.insert(str);
}

bool SetLexicon::has_string(const std::string& str) const
{
  return set_.find(str) != set_.end();
}

int SetLexicon::size() const
{
  return static_cast<int>(set_.size());
}

void SetLexicon::load(std::istream& instream)
{
  std::string line;
  while (std::getline(instream, line)) {
    add_string(line);
  }
}

void SetLexicon::dump(std::ostream& outstream) const
{
  for (const auto& str: set_) {
    outstream << str << "\n";
  }
}

std::set<std::string> SetLexicon::dump_strings() const
{
  std::set<std::string> strings;
  for (const auto& str: set_) {
    strings.insert(str);
  }
  return strings;
}

