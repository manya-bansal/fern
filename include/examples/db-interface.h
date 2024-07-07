#ifndef DB_INTERFACE_LIB
#define DB_INTERFACE_LIB

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "examples/common-ds.h"
#include "utils/name.h"

namespace examples {
class DbArray : public fern::AbstractDataStructure {
public:
  DbArray(const std::string &name) : name(name) {}

  DbArray() : DbArray(fern::util::uniqueName("array")) {}

  std::string getTypeName() const { return "image<float>"; }
  // This is essentially the "read function"
  std::string getDataQueryInterface() const { return {"dummy_query"}; }

  std::vector<std::string> getMetaData() const {
    return {"x", "y", "x_tile_size", "y_tile_size"};
  }

  std::string getDataInsertInterface() const { return {"insert_tile"}; }

  std::string getAllocData() const { return {"alloc_image"}; }

  std::string getVarName() const { return name; }

  std::string getQueryFreeInterface() const override { return "destroy"; }

  std::string getAllocFreeInterface() const override { return "destroy"; }

  bool isTranslationInvariant(int index) const {
    if (index < 2) {
      return true;
    }
    return false;
  }

private:
  std::string name;
};

class DbArrayProcessing : public fern::AbstractDataStructure {
public:
  DbArrayProcessing(const std::string &name) : name(name) {}

  DbArrayProcessing() : DbArrayProcessing(fern::util::uniqueName("array")) {}

  std::string getTypeName() const { return "image<float>"; }
  // This is essentially the "read function"
  std::string getDataQueryInterface() const { return {"get_tile"}; }

  std::vector<std::string> getMetaData() const {
    return {"x", "y", "x_tile_size", "y_tile_size"};
  }

  std::string getDataInsertInterface() const { return {"insert_tile"}; }

  std::string getAllocData() const { return {"alloc_image"}; }

  std::string getVarName() const { return name; }

  std::string getAllocFreeInterface() const override { return "destroy"; }

  bool isTranslationInvariant(int index) const {
    if (index < 2) {
      return true;
    }
    return false;
  }

private:
  std::string name;
};

class SigmoidProcessing : public fern::AbstractFunctionCall {
public:
  SigmoidProcessing()
      : input(DbArray("input")), output(DbArrayProcessing("input")){};

  std::string getName() const override { return "sigmoid_processing"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, false);
    fern::Variable y("y", false, false, false);
    fern::DataStructure block_out("out", &output);
    fern::DataStructure block_input("in", &input);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, 0, output["logical_rows"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, 0, output["logical_cols"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x, y, x_tile, y_tile)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {new fern::DataStructureArg(fern::DataStructurePtr(&input)),
            new fern::DataStructureArg(fern::DataStructurePtr(&output))};
  }

  DbArray input;
  DbArrayProcessing output;
};

} // namespace examples

#endif