#ifndef PY_POINTWISE_ARRAY_H
#define PY_POINTWISE_ARRAY_H

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "utils/name.h"
#include "pointwise-array.h"
namespace examples {

// template <typename T> inline std::string getNameInternal() {
//   std::cout << "No matching function" << std::endl;
// }

// template <> inline std::string getNameInternal<float>() {
//   return "Array<float>";
// }

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

template <typename T> class PyMatrix : public fern::AbstractDataStructure {
public:
  PyMatrix() : name(fern::util::uniqueName("var")) {}
  PyMatrix(const std::string &name) : name(name) {}

  std::string getTypeName() const override { return "Tensor<float>"; }

  std::vector<std::string> getMetaData() const override {
    return {"H_idx", "W_idx", "H_len", "W_len"};
  }

  bool insertQueried() const override { return true; }

  std::string getDataQueryInterface() const override { return {"matrix_query"}; }

  std::string getDataInsertInterface() const override {
    return {"matrix_insert"};
  }

  std::string getAllocData() const override { return {"Tensor.empty"}; };

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
     return {fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&b))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::PyArray<float> a;
  examples::PyArray<float> b;
  examples::PyArray<float> out;
};

class PytorchVadd : public fern::AbstractFunctionCall {
public:
  PytorchVadd()
      : a(examples::PyArray<float>("a")), b(examples::PyArray<float>("b")),
        out(examples::PyArray<float>("out")) {};

  std::string getName() const override { return "torch.add"; }

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
     return {fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&a))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&b))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&out)))};
  }

  examples::PyArray<float> a;
  examples::PyArray<float> b;
  examples::PyArray<float> out;
};

class PytorchMadd : public fern::AbstractFunctionCall {
public:
  PytorchMadd()
      : a(examples::PyMatrix<float>("a")), b(examples::PyMatrix<float>("b")),
        out(examples::PyMatrix<float>("out")) {};

  std::string getName() const override { return "Tensor.add"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable i("i", false, false, false);
    fern::Variable j("j", false, false, false);
    fern::Variable i_len("i_len", false, false, false);
    fern::Variable j_len("j_len", false, false, false);
    fern::Interval i_loop(i, 0, out["size(dim=0)"], i_len);
    fern::Interval j_loop(j, 0, out["size(dim=1)"], j_len);
    fern::DataStructure block_out("out", &out);
    fern::DataStructure block_a("a", &a);
    fern::DataStructure block_b("b", &b);
	return j_loop(
		i_loop(
			fern::ComputationAnnotation(
				fern::Producer(block_out(i, j, i_len, j_len)),
				fern::Consumer({
					block_a(i, j, i_len, j_len),
					block_b(i, j, i_len, j_len)
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

  examples::PyMatrix<float> a;
  examples::PyMatrix<float> b;
  examples::PyMatrix<float> out;
};

class PytorchMmul : public fern::AbstractFunctionCall {
public:
  PytorchMmul()
      : a(examples::PyMatrix<float>("a")), b(examples::PyMatrix<float>("b")),
        out(examples::PyMatrix<float>("out")) {};

  std::string getName() const override { return "Tensor.matmul"; }

  fern::DependencySubset getDataRelationship() const override {
    // PARALLELIZE PRODUCTION BY X
    // this only shows up in the case that this is the last function
    fern::Variable i("i", false, false, false);
    fern::Variable j("j", false, false, false);
    fern::Variable i_len("i_len", false, false, false);
    fern::Variable j_len("j_len", false, false, false);
	fern::Variable shared_len("shared_len", false, false, false);
    fern::Interval i_loop(i, 0, out["size(dim=0)"], i_len);
    fern::Interval j_loop(j, 0, out["size(dim=1)"], j_len);
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

  examples::PyMatrix<float> a;
  examples::PyMatrix<float> b;
  examples::PyMatrix<float> out;
};

class PytorchTranspose : public fern::AbstractFunctionCall {
public:
  PytorchTranspose()
      : a(examples::PyMatrix<float>("a")),
        out(examples::PyMatrix<float>("out")) {};

  std::string getName() const override { return "Tensor.transpose"; }

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

  examples::PyMatrix<float> a;
  examples::PyMatrix<float> out;
};

class PytorchDivn : public fern::AbstractFunctionCall {
public:
  PytorchDivn()
      : a(examples::PyMatrix<float>("a")),
	  	n(examples::Float("n")),
        out(examples::PyMatrix<float>("out")) {};

  std::string getName() const override { return "Tensor.divn"; }

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

  examples::PyMatrix<float> a;
  Float n;
  examples::PyMatrix<float> out;
};

class PytorchSoftmax : public fern::AbstractFunctionCall {
public:
  PytorchSoftmax()
      : a(examples::PyMatrix<float>("a")),
        out(examples::PyMatrix<float>("out")) {};

  std::string getName() const override { return "Tensor.softmax"; }

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

  examples::PyMatrix<float> a;
  examples::PyMatrix<float> out;
};

} // namespace examples

#endif