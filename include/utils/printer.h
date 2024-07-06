#ifndef MY_PRINTER_H
#define MY_PRINTER_H

#include <fstream>
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

template <typename Object>
void printToFile(const Object &obj, const std::string &filename) {
  std::ofstream outFile(filename);
  if (!outFile) {
    std::cerr << "Unable to open file";
    return;
  }

  outFile << obj;
  outFile.close();
}

} // namespace util
} // namespace fern

#endif