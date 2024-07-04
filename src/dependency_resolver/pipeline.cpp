#include "dependency_resolver/pipeline.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "utils/printer.h"

namespace fern {

void Pipeline::constructPipeline() {

  buildDataFlowGraph();

  buildComputationGraph();

  // TODOD: ADD lcm node mergering here?
  // Let's just query twice for rn!

  buildNaivePipeline();
}

void Pipeline::buildNaivePipeline() {

  // we are going to flip this after we are done
  std::vector<FunctionType> queries;
  std::vector<FunctionType> compute;
  std::vector<FunctionType> free;

  int last_func_index = functions.size() - 1;

  std::map<const AbstractDataStructure *, std::string> names;

  for (int i = last_func_index; i >= 0; i--) {
    auto call = functions[i];
    std::cout << call << std::endl;
    std::vector<
        std::tuple<std::vector<DependencyExpr>, std::vector<DependencyExpr>>>
        dependencies;
    generateDependency(call, dependencies);

    for (auto a : call.getInputs()) {
      if (isIntermediate(a)) {
        // Skip because we are going to allocate it later
        continue;
      }

      // Always query twice in the case that it's an input (no LCM node merging)
      std::string queried_name = util::uniqueName(a->getVarName() + "_q_");

      // Otherwise query now
      auto deps = getCorrespondingDependency(call, a);

      if (deps.size() == 0) {
        continue;
      }

      queries.push_back(new QueryNode(a, deps, a->getVarName(), queried_name));
      // Add to free if required

      if (a->getQueryFreeInterface() != "__fern__not__defined__") {
        free.push_back(new FreeNode(a, queried_name));
      }

      names[a] = queried_name;
    }

    auto output = call.getOutput();
    std::string queried_name = output->getVarName() + "_q";
    auto deps = getCorrespondingDependency(call, output);
    if (isIntermediate(output)) {
      queries.push_back(new AllocateNode(output, deps, queried_name));
      if (output->getAllocFreeInterface() != "__fern__not__defined__") {
        free.push_back(new FreeNode(output, queried_name));
      }
    } else {
      queries.push_back(
          new QueryNode(output, deps, output->getVarName(), queried_name));
    }

    names[output] = queried_name;
    compute.push_back(new ComputeNode(call, names));
  }

  util::printIterable(queries);
  std::cout << std::endl;
  util::printIterable(compute);
  std::cout << std::endl;
  util::printIterable(free);
  std::cout << std::endl;
}

bool Pipeline::isIntermediate(const AbstractDataStructure *ds) {
  if (computation_graph.find(ds) == computation_graph.end()) {
    return false;
  }
  return functions[functions.size() - 1].getOutput() != ds;
}

void Pipeline::buildDataFlowGraph() {
  // Create a map of all the data structures and the
  // input output pairs that they depend on.
  for (const auto &funcs : functions) {
    dataflow_graph.push_back({funcs.getOutput(), funcs.getInputs()});
  }
}

void Pipeline::buildComputationGraph() {
  for (const auto &func : functions) {
    auto output = func.getOutput();
    FERN_ASSERT(computation_graph.find(output) == computation_graph.end(),
                "Output should never be assigned to twice!");
    computation_graph[output] = func;
  }
}

void Pipeline::generateDependency(
    ConcreteFunctionCall call,
    std::vector<std::tuple<std::vector<DependencyExpr>,
                           std::vector<DependencyExpr>>> &dependencies) {
  std::cout << "Calling generate dependency" << std::endl;
  auto inputs = call.getInputs();
  for (const auto &input : inputs) {
    // If input is not another functions output, then we are done,
    // the input should be ready already
    if (computation_graph.find(input) == computation_graph.end()) {
      continue;
    }

    // if (input)

    // Otherwise, look at the input subset requested
    auto output_requirement = getCorrespondingDependency(call, input);

    // and get the output subset that the function where the input originates
    // from produces
    auto input_requirement =
        getCorrespondingDependency(computation_graph[input], input);

    // Now, generate a query to the dependency solver to get the relationship
    // between the two.
    std::tuple<std::vector<DependencyExpr>, std::vector<DependencyExpr>> output(
        {}, output_requirement);
    std::tuple<std::vector<DependencyExpr>, std::vector<DependencyExpr>>
        input_next(input_requirement, {});
    SolveDependency dependency_solver({output, input_next});
    // std::cout << "here" << std::endl;

    dependency_solver.set_verbose();
    dependency_solver.solve();

    for (const auto &sol : dependency_solver.solutions) {
      var_relationships[sol.var] = sol.solution;
      defined.insert(sol.var);
    }

    // insert_data_depedendence(input, dependency_solver.solutions);

    for (const auto &undef : dependency_solver.undefined) {
      // Cannot find undefined in the set of variables that
      // were previously defined, so the variable must be
      // really undefined
      if (defined.find(undef) == defined.end()) {
        undefined.insert(undef);
      }
    }

    // // solve dependencies originating from the previous
    // // func calls.
    // JUST DO ONE FOR RN
    // generateDependency(computation_graph[input], dependencies);
  }
}

std::vector<DependencyExpr>
Pipeline::getCorrespondingDependency(ConcreteFunctionCall call,
                                     const AbstractDataStructure *ds,
                                     int arg_place) const {
  // Figure out which argument we are looking at if arg_place is not set
  const AbstractDataStructure *abstract_var;
  if (arg_place == -1) {
    abstract_var = corresponding_abstract_var(call, ds);
  } else {
    // std::cout << *ds << std::endl;
    // std::cout << filter << std::endl;
    // std::cout << call.getAbstractArguments()[arg_place] << std::endl;
    FERN_ASSERT(call.getAbstractArguments()[arg_place].getArgType() ==
                    DATASTRUCTURE,
                "Passed in argument is not of the right type");
    abstract_var = call.getAbstractArguments()[arg_place]
                       .getNode<DataStructureArg>()
                       ->dsPtr();
  }

  // std::cout << "abstract_var = " << *abstract_var << std::endl;
  // std::cout << "getDataRelationship = " << call.getDataRelationship() <<
  // std::endl;

  // get the constraint corresponding to the data structure.
  std::vector<DependencyExpr> matching_constraint;
  match(call.getDataRelationship(),
        std::function<void(const DataStructureNode *, Matcher *)>(
            [&](const DataStructureNode *op, Matcher *ctx) {
              if (abstract_var == op->abstractData) {
                matching_constraint = op->annotations;
              }
            }));

  // util::printIterable(matching_constraint);
  return matching_constraint;
}

const AbstractDataStructure *
Pipeline::corresponding_abstract_var(ConcreteFunctionCall call,
                                     const AbstractDataStructure *ds) const {
  auto arguments = call.getArguments();
  int found_at = -1;
  for (int i = 0; i < arguments.size(); i++) {
    if (arguments[i].getArgType() == DATASTRUCTURE) {
      auto arg = arguments[i].getNode<DataStructureArg>()->dsPtr();
      if (arg == ds) {
        found_at = i;
      }
    }
  }

  // Did not find, return nullptr
  if (found_at == -1) {
    // should never happen
    FERN_ASSERT(false, "Did not find a match!!");
    return nullptr;
  }

  auto abstract_arg = call.getAbstractArguments()[found_at];
  FERN_ASSERT((abstract_arg.getArgType() == DATASTRUCTURE),
              "Not a valid data structure argument");

  return abstract_arg.getNode<DataStructureArg>()->dsPtr();
}

} // namespace fern