#ifndef IMG_CONVS_H
#define IMG_CONVS_H

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "examples/common-ds.h"
#include "utils/name.h"

namespace examples {

class Weights : public fern::AbstractDataStructure {
public:
  Weights(const std::string &name) : name(name) {}

  Weights() : Weights(fern::util::uniqueName("weight")) {}

  std::string getTypeName() const { return "Weights<float>"; }

  // std::string getDataQueryInterface() const { return {"query_materialize"}; }

  std::string getDataQueryInterface() const { return {"query_materialize"}; }

  std::vector<std::string> getMetaData() const {
    return {"H_start", "W_start", "H_len", "W_len"};
  }

  std::string getDataInsertInterface() const { return {"insert_materialize"}; }

  std::string getAllocData() const { return {"weight_alloc"}; }

  std::string getVarName() const { return name; }

  std::string getAllocFreeInterface() const override { return "free_weight"; }
  std::string getQueryFreeInterface() const override { return "free_weight"; }

  bool isTranslationInvariant(int index) const {
    if (index < 2) {
      return true;
    }
    return false;
  }

  bool useAllocNotQuery() const override { return true; }

private:
  std::string name;
};

class Convolution_Mkl : public fern::AbstractFunctionCall {
public:
  Convolution_Mkl()
      : input(Weights("input")), filter(Weights("filter")),
        bias(FloatPtr("bias")), stride_arg(fern::Variable("stride_arg", true)),
        output(Weights("output")) {}

  std::string getName() const override { return "convolution_mkl"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &output);
    fern::DataStructure block_input("in", &input);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, output["W_start"],
                          output["W_start"] + output["W_len"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, output["H_start"],
                          output["H_start"] + output["H_len"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x * stride_arg, y * stride_arg,
                                    x_tile + filter["H"] - 1,
                                    y_tile + filter["W"] - 1)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {new fern::DataStructureArg(fern::DataStructurePtr(&input)),
            new fern::DataStructureArg(fern::DataStructurePtr(&filter)),
            new fern::DummyDataStructureArg(&bias),
            new fern::VariableArg(fern::getNode(stride_arg)),
            new fern::DataStructureArg(fern::DataStructurePtr(&output))};
  }

  Weights input;
  Weights filter;
  FloatPtr bias;
  fern::Variable stride_arg;
  Weights output;
};

class BiasAdd : public fern::AbstractFunctionCall {
public:
  BiasAdd()
      : input(Weights("input")), bias(FloatPtr("bias")),
        output(Weights("output")) {}

  std::string getName() const override { return "biasMaterial<float>"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &output);
    fern::DataStructure block_input("in", &input);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, output["W_start"],
                          output["W_start"] + output["LW"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, output["H_start"],
                          output["H_start"] + output["LH"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x, y, x_tile, y_tile)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        new fern::DataStructureArg(fern::DataStructurePtr(&input)),
        new fern::DummyDataStructureArg(&bias),
        new fern::DataStructureArg(fern::DataStructurePtr(&output)),
    };
  }

  Weights input;
  FloatPtr bias;
  Weights output;
};

class Relu_Material : public fern::AbstractFunctionCall {
public:
  Relu_Material() : input(Weights("input")), output(Weights("output")) {}

  std::string getName() const override { return "relu_material"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &output);
    fern::DataStructure block_input("in", &input);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, output["W_start"],
                          output["W_start"] + output["LW"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, output["H_start"],
                          output["H_start"] + output["LH"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x, y, x_tile, y_tile)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        new fern::DataStructureArg(fern::DataStructurePtr(&input)),
        new fern::DataStructureArg(fern::DataStructurePtr(&output)),
    };
  }

  Weights input;
  Weights output;
};

class Maxpool : public fern::AbstractFunctionCall {
public:
  Maxpool()
      : input(Weights("input")), output(Weights("output")),
        maxpool_dim(fern::Variable("maxpool_dim", true)) {}

  std::string getName() const override { return "maxpool"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &output);
    fern::DataStructure block_input("in", &input);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, output["W_start"],
                          output["W_start"] + output["LW"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, output["H_start"],
                          output["H_start"] + output["LH"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer(
            {block_input(x * maxpool_dim, y * maxpool_dim, x_tile * maxpool_dim,
                         y_tile * maxpool_dim)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        new fern::DataStructureArg(fern::DataStructurePtr(&input)),
        new fern::VariableArg(fern::getNode(maxpool_dim)),
        new fern::DataStructureArg(fern::DataStructurePtr(&output)),
    };
  }

  Weights input;
  Weights output;
  fern::Variable maxpool_dim;
};

class Convolution_Mkl_Fused : public fern::AbstractFunctionCall {
public:
  Convolution_Mkl_Fused()
      : input(Weights("input")), filter(Weights("filter")),
        bias(FloatPtr("bias")), stride_arg(fern::Variable("stride_arg", true)),
        output(Weights("output")) {}

  std::string getName() const override { return "conv_new_mkl"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &output);
    fern::DataStructure block_input("in", &input);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, output["W_start"],
                          output["W_start"] + output["W_len"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, output["H_start"],
                          output["H_start"] + output["H_len"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x * stride_arg, y * stride_arg,
                                    x_tile + filter["H"] - 1,
                                    y_tile + filter["W"] - 1)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {new fern::DataStructureArg(fern::DataStructurePtr(&input)),
            new fern::DataStructureArg(fern::DataStructurePtr(&filter)),
            new fern::DummyDataStructureArg(&bias),
            new fern::VariableArg(fern::getNode(stride_arg)),
            new fern::DataStructureArg(fern::DataStructurePtr(&output))};
  }

  Weights input;
  Weights filter;
  FloatPtr bias;
  fern::Variable stride_arg;
  Weights output;
};

class Convolution_NoFuse : public fern::AbstractFunctionCall {
public:
  Convolution_NoFuse()
      : input(Weights("input")), filter(Weights("filter")),
        bias(FloatPtr("bias")), stride_arg(fern::Variable("stride_arg", true)),
        output(Weights("output")) {}

  std::string getName() const override { return "conv_no_fern_mkl"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &output);
    fern::DataStructure block_input("in", &input);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, output["W_start"],
                          output["W_start"] + output["W_len"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, output["H_start"],
                          output["H_start"] + output["H_len"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x * stride_arg, y * stride_arg,
                                    x_tile + filter["H"] - 1,
                                    y_tile + filter["W"] - 1)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {new fern::DataStructureArg(fern::DataStructurePtr(&input)),
            new fern::DataStructureArg(fern::DataStructurePtr(&filter)),
            new fern::DummyDataStructureArg(&bias),
            new fern::VariableArg(fern::getNode(stride_arg)),
            new fern::DataStructureArg(fern::DataStructurePtr(&output))};
  }

  Weights input;
  Weights filter;
  FloatPtr bias;
  fern::Variable stride_arg;
  Weights output;
};

class Tanh : public fern::AbstractFunctionCall {
public:
  Tanh() : input(Weights("input")), output(Weights("output")) {}

  std::string getName() const override { return "tanh"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &output);
    fern::DataStructure block_input("in", &input);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, output["W_start"],
                          output["W_start"] + output["LW"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, output["H_start"],
                          output["H_start"] + output["LH"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x, y, x_tile, y_tile)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        new fern::DataStructureArg(fern::DataStructurePtr(&input)),
        new fern::DataStructureArg(fern::DataStructurePtr(&output)),
    };
  }

  Weights input;
  Weights output;
};

} // namespace examples

#endif