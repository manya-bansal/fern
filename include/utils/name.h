#ifndef UTIL_NAME_H
#define UTIL_NAME_H

#include <atomic>
#include <sstream>
#include <string>

namespace fern {
namespace util {

static std::atomic<int> uniqueNameCounter = 0;

static inline int uniqueCount() { return uniqueNameCounter++; }

static inline std::string uniqueName(char prefix) {
  return prefix + std::to_string(uniqueCount());
}

static inline std::string uniqueName(const std::string &prefix) {
  return prefix + std::to_string(uniqueCount());
}

static inline int getUniqueId() { return uniqueCount(); }
} // namespace util
} // namespace fern

#endif