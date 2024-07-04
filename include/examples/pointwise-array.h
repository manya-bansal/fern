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

template <typename T> class Array : public fern::AbstractDataStructure {
public:
  Array() : name(fern::util::uniqueName("var")) {}
  Array(const std::string &name) : name(name) {}

  std::string getTypeName() const { return getNameInternal<T>(); }

  std::vector<std::string> getMetaData() const { return {"idx", "len"}; }

  std::string getDataQueryInterface() const {
    return {"array_query_no_materialize"};
  }

  std::string getDataInsertInterface() const { return {""}; }

  std::string getAllocData() const { return {"array_alloc<float>"}; };

  std::string getVarName() const { return name; }

  std::string getAllocFreeInterface() const override { return "destroy"; }
  // std::string getQueryFreeInterface() const override { return "destroy"; }

private:
  std::string name;
};

class vadd : public fern::AbstractFunctionCall {
public:
  vadd()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        out(examples::Array<float>("out")){};

  std::string getName() const override { return "vadd"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, true);
    fern::Variable len("len", true);
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

  examples::Array<float> a;
  examples::Array<float> b;
  examples::Array<float> out;
};

class vmul : public fern::AbstractFunctionCall {
public:
  vmul()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        out(examples::Array<float>("out")){};

  std::string getName() const override { return "vmul"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, true);
    fern::Variable len("len", true);
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

  examples::Array<float> a;
  examples::Array<float> b;
  examples::Array<float> out;
};

class vsub : public fern::AbstractFunctionCall {
public:
  vsub()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        out(examples::Array<float>("out")){};

  std::string getName() const override { return "vsub"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, true);
    fern::Variable len("len", true);
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

  examples::Array<float> a;
  examples::Array<float> b;
  examples::Array<float> out;
};

class subi_ispc : public fern::AbstractFunctionCall {
public:
  subi_ispc()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "ispc::subi_ispc"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, true);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    fern::DataStructure block_b("b", &b);
    return loop(fern::ComputationAnnotation(fern::Producer(block_out(x, len)),
                                            fern::Consumer({
                                                block_a(x, len),
                                                // block_b(x, len),
                                            })));
  }

  std::vector<fern::Argument> getArguments() override {
    return {new fern::DataStructureArg(fern::DataStructurePtr(&a), "data"),
            new fern::LiteralArg(fern::Datatype::Float32, 0.0f),
            new fern::VariableArg(fern::getNode(len)),
            new fern::DataStructureArg(fern::DataStructurePtr(&out), "data")};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class divi_ispc : public fern::AbstractFunctionCall {
public:
  divi_ispc()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "ispc::divi_ispc"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, true);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    fern::DataStructure block_b("b", &b);
    return loop(fern::ComputationAnnotation(fern::Producer(block_out(x, len)),
                                            fern::Consumer({
                                                block_a(x, len),
                                                // block_b(x, len),
                                            })));
  }

  std::vector<fern::Argument> getArguments() override {
    return {new fern::DataStructureArg(fern::DataStructurePtr(&a)),
            new fern::LiteralArg(fern::Datatype::Float32, 0.0f),
            new fern::VariableArg(fern::getNode(len)),
            new fern::DataStructureArg(fern::DataStructurePtr(&out))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class muli_ispc : public fern::AbstractFunctionCall {
public:
  muli_ispc()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")) {
    fern::Variable x("x", false, false, true);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    fern::DataStructure block_b("b", &b);
    dependendy = loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, len)), fern::Consumer({
                                               block_a(x, len),
                                               // block_b(x, len),
                                           })));
  };

  std::string getName() const override { return "ispc::muli_ispc"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    return dependendy;
  }

  std::vector<fern::Argument> getArguments() override {
    return {new fern::DataStructureArg(fern::DataStructurePtr(&a)),
            new fern::LiteralArg(fern::Datatype::Float32, 0.0f),
            new fern::VariableArg(fern::getNode(len)),
            new fern::DataStructureArg(fern::DataStructurePtr(&out))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
  fern::DependencySubset dependendy;
};

class sin_ispc : public fern::AbstractFunctionCall {
public:
  sin_ispc()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "ispc::sin_ispc"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, true);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    fern::DataStructure block_b("b", &b);
    return loop(fern::ComputationAnnotation(fern::Producer(block_out(x, len)),
                                            fern::Consumer({
                                                block_a(x, len),
                                                // block_b(x, len),
                                            })));
  }

  std::vector<fern::Argument> getArguments() override {
    return {new fern::DataStructureArg(fern::DataStructurePtr(&a)),
            new fern::VariableArg(fern::getNode(len)),
            new fern::DataStructureArg(fern::DataStructurePtr(&out))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class asin_ispc : public fern::AbstractFunctionCall {
public:
  asin_ispc()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "ispc::asin_ispc"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, true);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    fern::DataStructure block_b("b", &b);
    return loop(fern::ComputationAnnotation(fern::Producer(block_out(x, len)),
                                            fern::Consumer({
                                                block_a(x, len),
                                                // block_b(x, len),
                                            })));
  }

  std::vector<fern::Argument> getArguments() override {
    return {new fern::DataStructureArg(fern::DataStructurePtr(&a)),
            new fern::VariableArg(fern::getNode(len)),
            new fern::DataStructureArg(fern::DataStructurePtr(&out))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class sqrt_ispc : public fern::AbstractFunctionCall {
public:
  sqrt_ispc()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "ispc::sqrt_ispc"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, true);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    fern::DataStructure block_b("b", &b);
    return loop(fern::ComputationAnnotation(fern::Producer(block_out(x, len)),
                                            fern::Consumer({
                                                block_a(x, len),
                                                // block_b(x, len),
                                            })));
  }

  std::vector<fern::Argument> getArguments() override {
    return {new fern::DataStructureArg(fern::DataStructurePtr(&a)),
            new fern::VariableArg(fern::getNode(len)),
            new fern::DataStructureArg(fern::DataStructurePtr(&out))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class cos_ispc : public fern::AbstractFunctionCall {
public:
  cos_ispc()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "ispc::cos_ispc"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, true);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    fern::DataStructure block_b("b", &b);
    return loop(fern::ComputationAnnotation(fern::Producer(block_out(x, len)),
                                            fern::Consumer({
                                                block_a(x, len),
                                                // block_b(x, len),
                                            })));
  }

  std::vector<fern::Argument> getArguments() override {
    return {new fern::DataStructureArg(fern::DataStructurePtr(&a)),
            new fern::VariableArg(fern::getNode(len)),
            new fern::DataStructureArg(fern::DataStructurePtr(&out))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class mul_ispc : public fern::AbstractFunctionCall {
public:
  mul_ispc()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "ispc::mul_ispc"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, true);
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
            new fern::VariableArg(fern::getNode(len)),
            new fern::DataStructureArg(fern::DataStructurePtr(&out))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class add_ispc : public fern::AbstractFunctionCall {
public:
  add_ispc()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "ispc::add_ispc"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, true);
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
            new fern::VariableArg(fern::getNode(len)),
            new fern::DataStructureArg(fern::DataStructurePtr(&out))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class muli_ispc_8 : public fern::AbstractFunctionCall {
public:
  muli_ispc_8()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        out(examples::Array<float>("out")) {
    fern::Variable x("x", false, false, true);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], 8);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    fern::DataStructure block_b("b", &b);
    dependendy = loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, 8)), fern::Consumer({
                                             block_a(x, 8),
                                             // block_b(x, len),
                                         })));
  };

  std::string getName() const override { return "ispc::muli_ispc"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    return dependendy;
  }

  std::vector<fern::Argument> getArguments() override {
    return {new fern::DataStructureArg(fern::DataStructurePtr(&a)),
            new fern::LiteralArg(fern::Datatype::Float32, 0.0f),
            new fern::LiteralArg(fern::Datatype::Int64, 8),
            new fern::DataStructureArg(fern::DataStructurePtr(&out))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  examples::Array<float> out;
  fern::DependencySubset dependendy;
};

} // namespace examples

#endif