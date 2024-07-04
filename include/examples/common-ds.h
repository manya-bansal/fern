#ifndef COMMON_DS_H
#define COMMON_DS_H

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "utils/name.h"

namespace examples {

class FloatPtr : public fern::DummyDataStructure {
public:
  FloatPtr(const std::string &name) : name(name) {}

  std::string getTypeName() const { return "float*"; }

  std::string getVarName() const { return name; }

  void setRef(bool r) { ref = r; }

  void setConst(bool c) { is_const = c; }

  int numRef() const { return ref; }

  bool isConst() const { return is_const; }

private:
  std::string name;
  int ref = 0;
  bool is_const = false;
};

class DummyWeights : public fern::DummyDataStructure {
public:
  DummyWeights(const std::string &name) : name(name) {}

  std::string getTypeName() const { return " Weights<float>"; }

  std::string getVarName() const { return name; }

  void setRef(bool r) { ref = r; }

  void setConst(bool c) { is_const = c; }

  int numRef() const { return ref; }

  bool isConst() const { return is_const; }

private:
  std::string name;
  int ref = 0;
  bool is_const = false;
};

} // namespace examples

#endif