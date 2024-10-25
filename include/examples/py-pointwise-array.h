#ifndef POINTWISE_ARRAY_H
#define POINTWISE_ARRAY_H

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "utils/name.h"
namespace examples {

template <typename T> inline std::string getNameInternal() {
  std::cout << "No matching function" << std::endl;
}

template <> inline std::string getNameInternal<float>() {
  return "Array<float>";
}

template <typename T> class PyArray : public fern::AbstractDataStructure {
public:
  PyArray() : name(fern::util::uniqueName("var")) {}
  PyArray(const std::string &name) : name(name) {}

  std::string getTypeName() const override { return getNameInternal<T>(); }

  std::vector<std::string> getMetaData() const override {
    return {"idx", "len"};
  }

  bool insertQueried() const override { return true; }

  std::string getDataQueryInterface() const override { return {"array_query"}; }

  std::string getDataInsertInterface() const override {
    return {"array_insert"};
  }

  std::string getAllocData() const override { return {"Array"}; };

  std::string getVarName() const override { return name; }

  bool isTranslationInvariant(int index) const override {
    if (index < 1) {
      return true;
    }
    return false;
  }

private:
  std::string name;
};

class PyVadd : public fern::AbstractFunctionCall {
public:
  PyVadd()
      : a(examples::PyArray<float>("a")), b(examples::PyArray<float>("b")),
        out(examples::PyArray<float>("out")) {};

  std::string getName() const override { return "vadd"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, false);
    fern::Variable len("len", false, false, false);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    fern::DataStructure block_b("b", &b);
    return loop(fern::ComputationAnnotation(fern::Producer(block_out(x, len)),
                                            fern::Consumer({
                                                block_a(x, len),
                                                block_b(x, len),
                                            })));
  }

  std::vector<fern::Argument> getArguments() override {
    return {new fern::DataStructureArg(fern::DataStructurePtr(&a)),
            new fern::DataStructureArg(fern::DataStructurePtr(&b)),
            new fern::DataStructureArg(fern::DataStructurePtr(&out))};
  }

  examples::PyArray<float> a;
  examples::PyArray<float> b;
  examples::PyArray<float> out;
};

} // namespace examples

#endif