#ifndef CAPS_SUCCINCT_BASE_H_
#define CAPS_SUCCINCT_BASE_H_

#include <string>

/**
 * Base class for a read-only succinct signal set.
 */
class SuccinctGraphBase
{
 public:

  // Use the default values for
  SuccinctGraphBase() = default;
  SuccinctGraphBase(SuccinctGraphBase& orig) = default;
  SuccinctGraphBase(SuccinctGraphBase&& orig) = default;
  SuccinctGraphBase& operator=(SuccinctGraphBase rhs) = default;
  virtual ~SuccinctGraphBase() = default;

  virtual void load_file(std::string filename) = 0;
  virtual bool in_set(std::string str) = 0;
};

#endif  // CAPS_SUCCINCT_BASE_H_
