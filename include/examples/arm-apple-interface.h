#ifndef ARM_APPLE_INTERFACE_H
#define ARM_APPLE_INTERFACE_H

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "examples/common-ds.h"
#include "utils/name.h"

#include <string>

namespace examples {

template <typename T> class Matrix : public fuse::AbstractDataStructure {
public:
  Matrix() : name(fuse::util::uniqueName("var")) {}
  Matrix(const std::string &name) : name(name) {}

  std::string getTypeName() const { return getNameInternal<T>(); }

  std::vector<std::string> getMetaData() const {
    return {"x", "y", "x_len", "y_len"};
  }

  // Hacky, but easy to not to do this stupidity MANYA
  // fix
  std::string getDataQueryInterface() const { return {"matrix_query"}; }

  std::string getDataInsertInterface() const { return {"matrix_insert"}; }

  std::string getAllocData() const { return {"matrix"}; };

  std::string getVarName() const { return name; }

  std::string getAllocFreeInterface() const override { return "destroy"; }
  std::string getQueryFreeInterface() const override { return "destroy"; }

private:
  std::string name;
};

} // namespace examples

#endif