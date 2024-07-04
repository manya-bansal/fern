#ifndef HALIDE_EXAMPLES_H
#define HALIDE_EXAMPLES_H

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "examples/common-ds.h"
#include "utils/name.h"

namespace examples {

class image : public fern::AbstractDataStructure {
public:
  image(const std::string &name) : name(name) {}

  image() : image(fern::util::uniqueName("weight")) {}

  std::string getTypeName() const { return "image<float>"; }

  std::string getDataQueryInterface() const { return {"get_tile"}; }

  std::vector<std::string> getMetaData() const {
    return {"x", "y", "x_tile_size", "y_tile_size"};
  }

  std::string getDataInsertInterface() const { return {"insert_tile"}; }

  std::string getAllocData() const { return {"alloc_image"}; }

  std::string getVarName() const { return name; }

  std::string getAllocFreeInterface() const override { return "destroy"; }

private:
  std::string name;
};

class blur_x : public fern::AbstractFunctionCall {
public:
  blur_x() : img(image("img")), result(image("result")) {}

  std::string getName() const override { return "naive_blur_x_take_2_vec"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &result);
    fern::DataStructure block_input("in", &img);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, 0, result["logical_rows"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, 0, result["logical_cols"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x, y, x_tile, y_tile + 2)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        new fern::DataStructureArg(fern::DataStructurePtr(&img)),
        new fern::DataStructureArg(fern::DataStructurePtr(&result)),
    };
  }

  image img;
  image result;
};

class blur_y : public fern::AbstractFunctionCall {
public:
  blur_y() : img(image("img")), result(image("result")) {}

  std::string getName() const override { return "naive_blur_y_take_2_vec"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &result);
    fern::DataStructure block_input("in", &img);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, 0, result["logical_rows"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, 0, result["logical_cols"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x, y, x_tile + 2, y_tile)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        new fern::DataStructureArg(fern::DataStructurePtr(&img)),
        new fern::DataStructureArg(fern::DataStructurePtr(&result)),
    };
  }

  image img;
  image result;
};

class addition : public fern::AbstractFunctionCall {
public:
  addition() : img(image("img")), result(image("result")) {}

  std::string getName() const override { return "addition"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &result);
    fern::DataStructure block_input("in", &img);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, 0, result["logical_rows"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, 0, result["logical_cols"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x, y, x_tile, y_tile)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        new fern::DataStructureArg(fern::DataStructurePtr(&img)),
        new fern::DataStructureArg(fern::DataStructurePtr(&result)),
    };
  }

  image img;
  image result;
};

} // namespace examples

#endif