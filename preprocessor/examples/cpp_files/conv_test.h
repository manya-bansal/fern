#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "examples/common-ds.h"
#include "utils/name.h"


namespace examples {

class convolution_mkl_test : public fern::AbstractFunctionCall {

public:
	std::string getName() const override { return {"convolution_mkl_test"}; }

	fern::DependencySubset getDataRelationship() const override {
	fern::Variable x_tile("x_tile",false, false, false);
	fern::Variable y_tile("y_tile",false, false, false);
	fern::Variable stride_arg("stride_arg",false, false, false);
	fern::Variable x("x", false, false, false);
	fern::Interval x_loop(x, output["W_start"], output["W_start"]+output["W_len"], x_tile);
	fern::Variable y("y", false, false, false);
	fern::Interval y_loop(y, output["H_start"], output["H_start"]+output["H_len"], y_tile);
	fern::DataStructure block_output("block_output", &output);
	fern::DataStructure block_input("block_input", &input);
	return x_loop(y_loop(fern::ComputationAnnotation(
		fern::Producer(block_output(x,y,x_tile,y_tile)),
		fern::Consumer({block_input(x*stride_arg,y*stride_arg,x_tile+filter["H"]-1,x_tile+filter["W"]-1)})))); }

	std::vector<fern::Argument> getArguments() override {
		return { new fern::DataStructureArg(fern::DataStructurePtr(&input)),
		new fern::DataStructureArg(fern::DataStructurePtr(&filter)),
		new fern::DataStructureArg(fern::DataStructurePtr(&bias)),
		new fern::VariableArg(fern::getNode(stride_arg)),
		new fern::DataStructureArg(fern::DataStructurePtr(&output)) }; }

Weights input;
Weights filter;
Weights bias;
fern::Variable stride_arg;
Weights output;
};



}