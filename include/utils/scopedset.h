// From TACO

#ifndef FERN_UTIL_SCOPEDSET_H
#define FERN_UTIL_SCOPEDSET_H

#include <list>
#include <ostream>
#include <set>

#include "fuse_error.h"

namespace fern {
namespace util {

template <typename Key> class ScopedSet {
public:
  ScopedSet() { scope(); }

  ~ScopedSet() { unscope(); }

  /// Add a level of scoping.
  void scope() { scopes.push_front(std::set<Key>()); }

  /// Remove a level of scoping.
  void unscope() { scopes.pop_front(); }

  void insert(const Key &key) { scopes.front().insert(key); }

  void remove(const Key &key) {
    for (auto &scope : scopes) {
      const auto it = scope.find(key);
      if (it != scope.end()) {
        scope.erase(it);
        return;
      }
    }
    FERN_ASSERT(false, "not in scope!");
  }

  bool contains(const Key &key) {
    for (auto &scope : scopes) {
      if (scope.find(key) != scope.end()) {
        return true;
      }
    }
    return false;
  }

  friend std::ostream &operator<<(std::ostream &os, ScopedSet<Key> sset) {
    os << "ScopedSet:" << std::endl;

    for (auto &scope : sset.scopes) {
      os << "Scope{";
      for (auto &elem : scope) {
        os << elem << ", ";
      }
      os << "}" << std::endl;
    }

    return os;
  }

private:
  std::list<std::set<Key>> scopes;
};

} // namespace util
} // namespace fern
#endif
