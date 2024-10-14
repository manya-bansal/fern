#ifndef DEMO_H
#define DEMO_H

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "utils/name.h"
#include "examples/pointwise-array.h"
// Just some demos
namespace examples {

class kernel_1_whole : public fern::AbstractFunctionCall {
public:
  kernel_1_whole()
      : a(examples::Array<float>("a")),
        out(examples::Array<float>("out")){};

  std::string getName() const override { return "kernel_1_whole"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, false);
    fern::Variable len("len", false, false, false);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    return loop(fern::ComputationAnnotation(fern::Producer(block_out(x, len)),
                                            fern::Consumer({
                                                block_a(x, len),
                                            })));
  }

  std::vector<fern::Argument> getArguments() override {
      return {fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  Array<float> a;
  Array<float> out;
};

class kernel_2_whole : public fern::AbstractFunctionCall {
public:
  kernel_2_whole()
      : a(examples::Array<float>("a")),
        out(examples::Array<float>("out")){};

  std::string getName() const override { return "kernel_2_whole"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, false);
    fern::Variable len("len", false, false, false);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    return loop(fern::ComputationAnnotation(fern::Producer(block_out(x, len)),
                                            fern::Consumer({
                                                block_a(x, len),
                                            })));
  }

  std::vector<fern::Argument> getArguments() override {
    return {fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  Array<float> a;
  Array<float> out;
};

}

#endif