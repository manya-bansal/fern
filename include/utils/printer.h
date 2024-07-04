#ifndef MY_PRINTER_H
#define MY_PRINTER_H

#include <iostream>
#include <iterator>

namespace fern {
namespace util {

template <typename Iterable> void printIterable(const Iterable &iterable) {
  // Using auto to automatically deduce the type of elements
  for (const auto &element : iterable) {
    std::cout << element << " " << std::endl;
  }
  std::cout << std::endl;
}

} // namespace util
} // namespace fern

#endif