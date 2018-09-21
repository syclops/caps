/**
 * Author: smaptas
 */


// Include header file.
#include "lexicon.h"

// Include C standard libraries.

// Include C++ standard libraries.
#include <algorithm>
#include <iostream>
#include <set>
#include <string>

// Include other headers from this project.

// Include header from other projects.

Lexicon::Lexicon()
  : size_{0}
{
  // Nothing to do here.
}

Lexicon::~Lexicon()
{
  // Nothing to do here.
}

int Lexicon::size() const
{
  return size_;
}

void Lexicon::add_file(std::istream& instream)
{
  std::string line;
  while (std::getline(instream, line)) {
    add_string(line);
  }
}

bool operator==(const Lexicon& lhs, const Lexicon& rhs)
{
  if (lhs.size() != rhs.size()) {
    return false;
  }
  auto lhs_strings = lhs.dump_strings(), rhs_strings = rhs.dump_strings();
  return std::equal(lhs_strings.begin(), lhs_strings.end(),
                    rhs_strings.begin());
}

bool operator!=(const Lexicon& lhs, const Lexicon& rhs)
{
  return !operator==(lhs, rhs);
}
