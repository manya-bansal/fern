#include "dependency_resolver/pipeline.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "dependency_lang/dep_lang_rewriter.h"
#include "utils/printer.h"
#include <algorithm>

namespace fern {

std::ostream &operator<<(std::ostream &os, const IntervalPipe &i) {
  os << i.var << " in (" << i.start << ", " << i.end << ", " << i.step << ")"
     << std::endl;
  return os;
}

void Pipeline::constructPipeline() {

  buildDataFlowGraph();

  buildComputationGraph();

  // TODOD: ADD lcm node mergering here?
  // Let's just query twice for rn!

  buildNaivePipeline();
}

void Pipeline::buildNaivePipeline() {
  buildFuncCalls();
  generateOuterLoops();
}

void Pipeline::buildFuncCalls() {

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

  // Now, let's flip and store into pipeline
  for (auto elem = queries.rbegin(); elem != queries.rend(); ++elem) {
    pipeline.push_back(*elem);
  }

  for (auto elem = compute.rbegin(); elem != compute.rend(); ++elem) {
    pipeline.push_back(*elem);
  }

  for (auto elem = free.rbegin(); elem != free.rend(); ++elem) {
    pipeline.push_back(*elem);
  }

  // util::printIterable(queries);
  // std::cout << std::endl;
  // util::printIterable(compute);
  // std::cout << std::endl;
  // util::printIterable(free);
  // std::cout << std::endl;
}

static const AbstractDataStructure *
getConcreteVar(ConcreteFunctionCall call,
               const AbstractDataStructure *abstract_d) {

  auto arguments = call.getAbstractArguments();
  for (int i = 0; i < arguments.size(); i++) {
    auto arg = arguments[i];
    if (arg.getArgType() == DATASTRUCTURE) {
      auto a = arg.getNode<DataStructureArg>()->dsPtr();
      if (a->getVarName() == abstract_d->getVarName()) {
        return call.getArguments()[i].getNode<DataStructureArg>()->dsPtr();
      }
    }
  }

  // unreachable
  FERN_ASSERT_NO_MSG(false);
}

static DependencyExpr
replaceSelectedMetaNodes(DependencyExpr e,
                         const AbstractDataStructure *to_replace,
                         const AbstractDataStructure *new_ds) {
  struct ReplaceRewriter : public DependencyRewriter {
    using DependencyRewriter::visit;
    ReplaceRewriter(const AbstractDataStructure *d_abstract,
                    const AbstractDataStructure *d_concrete)
        : d_abstract(d_abstract), d_concrete(d_concrete) {}

    void visit(const MetaDataNode *op) {
      if (op->ds == d_abstract) {
        expr = DependencyExpr(new MetaDataNode(op->name, d_concrete));
      } else {
        expr = DependencyExpr(op);
      }
    }

    const AbstractDataStructure *d_abstract;
    const AbstractDataStructure *d_concrete;
  };

  ReplaceRewriter rw(to_replace, new_ds);
  return rw.rewrite(e);
}

static DependencyExpr replaceAllMetaNodes(DependencyExpr e,
                                          ConcreteFunctionCall func) {
  DependencyExpr new_expr = e;
  match(e, std::function<void(const MetaDataNode *, Matcher *)>(
               [&](const MetaDataNode *op, Matcher *ctx) {
                 auto concrete = getConcreteVar(func, op->ds);
                 new_expr =
                     replaceSelectedMetaNodes(new_expr, op->ds, concrete);
               }));

  return new_expr;
}

void Pipeline::generateOuterLoops() {
  // Get the last func and generate the loops with respect to the output
  auto last_func = functions[functions.size() - 1];

  match(last_func.getDataRelationship(),
        std::function<void(const IntervalNode *, Matcher *)>(
            [&](const IntervalNode *op, Matcher *ctx) {
              Variable v = op->var;
              DependencyExpr start = op->start;
              DependencyExpr end = op->end;
              DependencyExpr step = op->step;

              outer_loops.push_back(IntervalPipe(v, start, end, step));
              ctx->match(op->child);
            }));

  // util::printIterable(outer_loops);
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

std::ostream &operator<<(std::ostream &os, const Pipeline &pipe) {

  for (auto i : pipe.outer_loops) {
    os << i;
  }

  for (auto funcs : pipe.pipeline) {
    os << '\t' << funcs << std::endl;
  }

  return os;
}

// Scheduling operators

Pipeline Pipeline::reorder(int loop_1, int loop_2) {
  Pipeline new_pipe = *this;
  // Need to add a check for reorder
  // Make sure that you don't swap loops that depend on the values from
  // previous loops
  std::swap(new_pipe.outer_loops[loop_1], new_pipe.outer_loops[loop_2]);
  return new_pipe;
}

Pipeline Pipeline::split(int loop, Variable outer, Variable inner,
                         Variable outer_step, Variable inner_step) {
  Pipeline new_pipe = *this;
  IntervalPipe old_loop = new_pipe.outer_loops[loop];
  IntervalPipe outer_new(outer, old_loop.start, old_loop.end, outer_step);
  IntervalPipe inner_new(inner, outer, outer_step, inner_step);
  new_pipe.outer_loops[loop] = outer_new;
  new_pipe.outer_loops.insert(new_pipe.outer_loops.begin() + loop + 1,
                              inner_new);
  // std::swap(new_pipe.outer_loops[loop_1], new_pipe.outer_loops[loop_2]);
  return new_pipe;
}

Pipeline Pipeline::parrallelize(int loop) {
  Pipeline new_pipe = *this;
  new_pipe.outer_loops[loop].var.setParrallel();
  return new_pipe;
}

Pipeline Pipeline::bind(Variable var, int val) {
  Pipeline new_pipe = *this;
  var.setBound(val);
  new_pipe.bounded_vars.insert(var);
  return new_pipe;
}

} // namespace fern