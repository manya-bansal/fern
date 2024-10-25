#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "examples/common-ds.h"
#include "utils/name.h"


namespace examples {

class vadd : public fern::AbstractFunctionCall {

public:
	vadd() = default;
	std::string getName() const override { return {"vadd"}; }

	fern::DependencySubset getDataRelationship() const override {
	fern::Variable len(false, false, false);
	fern::Variable i(false, false, false);
	fern::Interval i_loop(i, A["idx"], A["idx"]+A["size"], len);
	fern::DataStructure block_A("block_A", &A);
	fern::DataStructure block_B("block_B", &B);
	fern::DataStructure block_C("block_C", &C);
	return i_loop(fern::ComputationAnnotation(
		fern::Producer(block_A(i,len)),
		fern::Consumer(block_B(i,len),
		block_C(i,len)))); }

	std::vector<fern::Argument> getArguments() override {
		return { new fern::DataStructureArg(fern::DataStructurePtr(&B)),
		new fern::DataStructureArg(fern::DataStructurePtr(&C)),
		new fern::DataStructureArg(fern::DataStructurePtr(&A)) }; }

Array<float> B;
Array<float> C;
Array<float> A;
};

class vsub : public fern::AbstractFunctionCall {

public:
	vsub() = default;
	std::string getName() const override { return {"vsub"}; }

	fern::DependencySubset getDataRelationship() const override {
	fern::Variable len(false, false, false);
	fern::Variable i(false, false, false);
	fern::Interval i_loop(i, A["idx"], A["idx"]+A["size"], len);
	fern::DataStructure block_A("block_A", &A);
	fern::DataStructure block_B("block_B", &B);
	fern::DataStructure block_C("block_C", &C);
	return i_loop(fern::ComputationAnnotation(
		fern::Producer(block_A(i,len)),
		fern::Consumer(block_B(i,len),
		block_C(i,len)))); }

	std::vector<fern::Argument> getArguments() override {
		return { new fern::DataStructureArg(fern::DataStructurePtr(&B)),
		new fern::DataStructureArg(fern::DataStructurePtr(&C)),
		new fern::DataStructureArg(fern::DataStructurePtr(&A)) }; }

Array<float> B;
Array<float> C;
Array<float> A;
};



}