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

  std::string getTypeName() const override { return getNameInternal<T>(); }

  std::vector<std::string> getMetaData() const override {
    return {"idx", "len"};
  }

  std::string getDataQueryInterface() const override {
    return {"array_query_no_materialize"};
  }

  std::string getDataInsertInterface() const override { return {""}; }

  std::string getAllocData() const override { return {"array_alloc<float>"}; };

  std::string getVarName() const override { return name; }

  std::string getAllocFreeInterface() const override { return "destroy"; }
  // std::string getQueryFreeInterface() const override { return "destroy"; }

  bool isTranslationInvariant(int index) const override {
    if (index < 1) {
      return true;
    }
    return false;
  }

private:
  std::string name;
};

template <typename T> class Matrix : public fern::AbstractDataStructure {
public:
  Matrix() : name(fern::util::uniqueName("var")) {}
  Matrix(const std::string &name) : name(name) {}

  std::string getTypeName() const override { return "Tensor<float>"; }

  std::vector<std::string> getMetaData() const override {
    return {"H_idx", "W_idx", "H_len", "W_len"};
  }

  bool insertQueried() const override { return false; }

  std::string getDataQueryInterface() const override { return {"matrix_query"}; }

  std::string getDataInsertInterface() const override {
    return {"matrix_insert"};
  }

  std::string getAllocData() const override { return {"Tensor<float>"}; };

  std::string getVarName() const override { return name; }

  bool isTranslationInvariant(int index) const override {
    if (index < 2) {
      return true;
    }
    return false;
  }

private:
  std::string name;
};

class Float : public fern::DummyDataStructure {
public:
  Float(const std::string &name) : name(name) {}

  std::string getTypeName() const { return "float"; }

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

class mmul : public fern::AbstractFunctionCall {
public:
  mmul()
      : a(examples::Matrix<float>("a")), b(examples::Matrix<float>("b")),
        out(examples::Matrix<float>("out")) {};

  std::string getName() const override { return "matmul"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable i("i", false, false, false);
    fern::Variable j("j", false, false, false);
    fern::Variable i_len("i_len", false, false, false);
    fern::Variable j_len("j_len", false, false, false);
	fern::Variable shared_len("shared_len", false, false, false);
    fern::Interval i_loop(i, 0, out["height"], i_len);
    fern::Interval j_loop(j, 0, out["width"], j_len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    fern::DataStructure block_b("b", &b);
	return j_loop(
		i_loop(
			fern::ComputationAnnotation(
				fern::Producer(block_out(i, j, i_len, j_len)),
				fern::Consumer({
					block_a(i, 0, i_len, shared_len),
					block_b(0, j, shared_len, j_len)
				})
			)
		)
	);
  }

  std::vector<fern::Argument> getArguments() override {
     return {fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&b))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Matrix<float> a;
  examples::Matrix<float> b;
  examples::Matrix<float> out;
};

class transpose : public fern::AbstractFunctionCall {
public:
  transpose()
      : a(examples::Matrix<float>("a")),
        out(examples::Matrix<float>("out")) {};

  std::string getName() const override { return "transpose"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable i("i", false, false, false);
    fern::Variable j("j", false, false, false);
    fern::Variable i_len("i_len", false, false, false);
    fern::Variable j_len("j_len", false, false, false);
    fern::Interval i_loop(i, 0, out["height"], i_len);
    fern::Interval j_loop(j, 0, out["width"], j_len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
	return j_loop(
		i_loop(
			fern::ComputationAnnotation(
				fern::Producer(block_out(i, j, i_len, j_len)),
				fern::Consumer({
					block_a(j, i, j_len, i_len),
				})
			)
		)
	);
  }

  std::vector<fern::Argument> getArguments() override {
     return {fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Matrix<float> a;
  examples::Matrix<float> out;
};

class divn : public fern::AbstractFunctionCall {
public:
  divn()
      : a(examples::Matrix<float>("a")),
	  	n(examples::Float("n")),
        out(examples::Matrix<float>("out")) {};

  std::string getName() const override { return "divn"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable i("i", false, false, false);
    fern::Variable j("j", false, false, false);
    fern::Variable i_len("i_len", false, false, false);
    fern::Variable j_len("j_len", false, false, false);
    fern::Interval i_loop(i, 0, out["height"], i_len);
    fern::Interval j_loop(j, 0, out["width"], j_len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
	return j_loop(
		i_loop(
			fern::ComputationAnnotation(
				fern::Producer(block_out(i, j, i_len, j_len)),
				fern::Consumer({
					block_a(i, j, i_len, j_len),
				})
			)
		)
	);
  }

  std::vector<fern::Argument> getArguments() override {
     return {fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&a))),
	        // fern::Argument(std::make_shared<fern::LiteralArg>(fern::Datatype::Float32, 0.0f)),
	 		fern::Argument(std::make_shared<fern::DummyDataStructureArg>(fern::DummyDataStructurePtr(&n))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Matrix<float> a;
  Float n;
  examples::Matrix<float> out;
};

class softmax : public fern::AbstractFunctionCall {
public:
  softmax()
      : a(examples::Matrix<float>("a")),
        out(examples::Matrix<float>("out")) {};

  std::string getName() const override { return "softmax"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable i("i", false, false, false);
    fern::Variable i_len("i_len", false, false, false);
	fern::Variable width("width", false, false, false);
    fern::Interval i_loop(i, 0, out["height"], i_len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
	return i_loop(
		fern::ComputationAnnotation(
			fern::Producer(block_out(i, 0, i_len, width)),
			fern::Consumer({
				block_a(i, 0, i_len, width),
			})
		)
	);
  }

  std::vector<fern::Argument> getArguments() override {
     return {fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Matrix<float> a;
  examples::Matrix<float> out;
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&b))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
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
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&b))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
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
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&b))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
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
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&a), "data")),
            fern::Argument(std::make_shared<fern::LiteralArg>(fern::Datatype::Float32, 0.0f)),
            fern::Argument(std::make_shared<fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&out), "data"))};
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
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::LiteralArg>(fern::Datatype::Float32, 0.0f)),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
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
    fern::Variable x("x", false, false, false);
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
    return { fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared<fern::LiteralArg>(fern::Datatype::Float32, 0.0f)),
            fern::Argument(std::make_shared<fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
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
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
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
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
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
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
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
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
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
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&b))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
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
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&b))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
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
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::LiteralArg>(fern::Datatype::Float32, 0.0f)),
            fern::Argument(std::make_shared< fern::LiteralArg>(fern::Datatype::Int64, 8)),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  examples::Array<float> out;
  fern::DependencySubset dependendy;
};

class addi_ispc : public fern::AbstractFunctionCall {
public:
  addi_ispc()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")) {
    fern::Variable x("x", false, false, false);
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

  std::string getName() const override { return "ispc::addi_ispc"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    return dependendy;
  }

  std::vector<fern::Argument> getArguments() override {
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::LiteralArg>(fern::Datatype::Float32, 0.0f)),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
  fern::DependencySubset dependendy;
};

class addi_ispc_extended_dep : public fern::AbstractFunctionCall {
public:
  addi_ispc_extended_dep()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")) {
    fern::Variable x("x", false, false, false);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    fern::DataStructure block_b("b", &b);
    dependendy = loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, len)), fern::Consumer({
                                               block_a(x + 6, len),
                                               // block_b(x, len),
                                           })));
  };

  std::string getName() const override { return "ispc::addi_ispc"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    return dependendy;
  }

  std::vector<fern::Argument> getArguments() override {
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::LiteralArg>(fern::Datatype::Float32, 0.0f)),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
  fern::DependencySubset dependendy;
};

// mock array function calls

class every_other : public fern::AbstractFunctionCall {
public:
  every_other()
      : a(examples::Array<float>("a")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "mock::every_other"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, false);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    return loop(fern::ComputationAnnotation(fern::Producer(block_out(x, (len + 1) / 2)),
                                            fern::Consumer({
                                                block_a(x, len),
                                                // block_b(x, len),
                                            })));
  }

  std::vector<fern::Argument> getArguments() override {
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class addi_mock : public fern::AbstractFunctionCall {
public:
  addi_mock()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "mock::addi"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::LiteralArg>(fern::Datatype::Float32, 0.0f)),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class subi_mock : public fern::AbstractFunctionCall {
public:
  subi_mock()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "mock::subi"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::LiteralArg>(fern::Datatype::Float32, 0.0f)),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class muli_mock : public fern::AbstractFunctionCall {
public:
  muli_mock()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")) {
    fern::Variable x("x", false, false, false);
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

  std::string getName() const override { return "mock::muli"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    return dependendy;
  }

  std::vector<fern::Argument> getArguments() override {
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::LiteralArg>(fern::Datatype::Float32, 0.0f)),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
  fern::DependencySubset dependendy;
};

class divi_mock : public fern::AbstractFunctionCall {
public:
  divi_mock()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "mock::divi"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::LiteralArg>(fern::Datatype::Float32, 0.0f)),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class torch_add : public fern::AbstractFunctionCall {
public:
  torch_add()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "mock::add"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&b))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class add_mock : public fern::AbstractFunctionCall {
public:
  add_mock()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "mock::add"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&b))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

// class addVec_mock : public fern::AbstractFunctionCall {
// public:
//   addVec_mock()
//       : len(fern::Variable("len", false)), out(examples::Array<float>("out")), a(std::vector<examples::Array<float>>()){};

//   std::string getName() const override { return "mock::addVec"; }

//   fern::DependencySubset getDataRelationship() const override {
//     // PARALLELIZE PRODUCTION BY X
//     // this only shows up in the case that this is the last function
//     fern::Variable x("x", false, false, false);
//     fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
//     fern::DataStructure block_out("out", &out);
// 	std::vector<fern::DependencySubset> consumer_blocks;
// 	for (int i = 0; i < a.size(); i++) {
// 		examples::Array<float> ai = a[i];
// 		fern::DataStructure block_i(std::to_string(i), &ai);
// 		fern::DependencySubset block = block_i(x, len);
// 		consumer_blocks.push_back(block);
// 	}
    
//     return loop(fern::ComputationAnnotation(fern::Producer(block_out(x, len)),
//                                             fern::Consumer(consumer_blocks)));
//   }

//   std::vector<fern::Argument> getArguments() override {
// 	std::vector<fern::Argument> args = {fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
//             fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
// 	for (int i = 0; i < a.size(); i++) {
// 		args.push_back(fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a[i]))));
// 	}
//     return args;
//   }

//   fern::Variable len;
//   examples::Array<float> out;
//   std::vector<examples::Array<float>> a;
// };

class sub_mock : public fern::AbstractFunctionCall {
public:
  sub_mock()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "mock::sub"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&b))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class mul_mock : public fern::AbstractFunctionCall {
public:
  mul_mock()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "mock::mul"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&b))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class div_mock : public fern::AbstractFunctionCall {
public:
  div_mock()
      : a(examples::Array<float>("a")), b(examples::Array<float>("b")),
        len(fern::Variable("len", false)), out(examples::Array<float>("out")){};

  std::string getName() const override { return "mock::div"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable x("x", false, false, false);
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
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&b))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  examples::Array<float> b;
  fern::Variable len;
  examples::Array<float> out;
};

class cdf_mock : public fern::AbstractFunctionCall {
public:
  cdf_mock()
      : a(examples::Array<float>("a")), len(fern::Variable("len", false)), out(examples::Array<float>("out")) {
    fern::Variable x("x", false, false, false);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    dependendy = loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, len)), fern::Consumer({
                                               block_a(x, len),
                                           })));
  };

  std::string getName() const override { return "mock::cdf"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    return dependendy;
  }

  std::vector<fern::Argument> getArguments() override {
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  fern::Variable len;
  examples::Array<float> out;
  fern::DependencySubset dependendy;
};

class log_mock : public fern::AbstractFunctionCall {
public:
  log_mock()
      : a(examples::Array<float>("a")), len(fern::Variable("len", false)), out(examples::Array<float>("out")) {
    fern::Variable x("x", false, false, false);
    fern::Interval loop(x, out["idx"], out["idx"] + out["size"], len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    dependendy = loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, len)), fern::Consumer({
                                               block_a(x, len),
                                           })));
  };

  std::string getName() const override { return "mock::vlog"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    return dependendy;
  }

  std::vector<fern::Argument> getArguments() override {
    return {fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared< fern::VariableArg>(fern::getNode(len))),
            fern::Argument(std::make_shared< fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::Array<float> a;
  fern::Variable len;
  examples::Array<float> out;
  fern::DependencySubset dependendy;
};

} // namespace examples

#endif