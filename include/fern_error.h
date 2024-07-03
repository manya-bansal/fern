#ifndef FERN_ERROR_H
#define FERN_ERROR_H

#include <iostream>
#include <sstream>

namespace fern {
// Custom assert macro
#define FERN_ASSERT(condition, message)                                        \
  do {                                                                         \
    if (!(condition)) {                                                        \
      std::stringstream ss;                                                    \
      ss << "Assertion failed: " << message << " in file " << __FILE__ << ":"  \
         << __LINE__;                                                          \
      auto e = std::runtime_error(ss.str());                                   \
      throw e;                                                                 \
    }                                                                          \
  } while (false)

// Custom assert macro
#define FERN_ASSERT_NO_MSG(condition)                                          \
  do {                                                                         \
    if (!(condition)) {                                                        \
      std::stringstream ss;                                                    \
      ss << "Assertion failed: in file " << __FILE__ << ":" << __LINE__;       \
      auto e = std::runtime_error(ss.str());                                   \
      throw e;                                                                 \
    }                                                                          \
  } while (false)
} // namespace fern

#endif