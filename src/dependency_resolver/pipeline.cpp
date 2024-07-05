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
      queries.push_back(new AllocateNode(output, deps, queried_name, call));
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

  std::cout << "In pipeline" << std::endl;

  for (auto i : pipe.outer_loops) {
    os << i;
  }

  for (auto d : pipe.derivations) {
    os << d.first << " = " << d.second << std::endl;
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
  new_pipe.derivations[old_loop.var] = outer + inner;
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

Pipeline Pipeline::finalize(bool hoist) {
  auto new_pipe = *this;
  if (hoist) {
    new_pipe.run_hoisting_pass();
  }

  return new_pipe;
}

static bool isTranslationInvariant(DependencyExpr e, DependencySubset rel) {

  bool is_invariant = true;
  match(e, std::function<void(const DependencyVariableNode *, Matcher *)>(
               [&](const DependencyVariableNode *op, Matcher *ctx) {
                 if (rel.is_interval_var(Variable(op))) {
                   is_invariant = false;
                 }
               }));
  return is_invariant;
}

bool Pipeline::hoist_able(const AllocateNode *node) {

  // Get the data relationships
  auto expr = getCorrespondingDependency(node->call, node->call.getOutput());
  // get all the interval nodes

  for (int i = 0; i < node->deps.size(); i++) {
    if (node->ds->isTranslationInvariant(i)) {
      // Nothing to check here
      continue;
    }
    // if not translation invariant make sure that it does
    // not contain any interval nodes
    if (!isTranslationInvariant(expr[i], node->call.getDataRelationship())) {
      return false;
    }
  }
  return true;
}

void Pipeline::run_hoisting_pass() {
  // First, look at the allocate functions and figure out whether
  // a candidate can be hoisted

  std::set<const AbstractDataStructure *> to_be_hoisted;
  for (auto p : pipeline) {
    if (p.getFuncType() == ALLOCATE) {
      auto p_alloc = p.getNode<AllocateNode>();
      if (hoist_able(p_alloc)) {
        // Add to hoisted
        to_be_hoisted.insert(p_alloc->ds);
      }
    }
  }

  // Now loop through all the functions again
  // and set pull out the allocs and frees

  std::vector<FunctionType> hoisted;
  std::vector<FunctionType> hoisted_free;

  for (int i = 0; i < pipeline.size(); i++) {
    auto p = pipeline[i];
    if (p.getFuncType() == ALLOCATE) {
      auto p_alloc = p.getNode<AllocateNode>()->ds;
      if (to_be_hoisted.count(p_alloc) > 0) {
        hoisted.push_back(p);
        pipeline[i] = new BlankNode();
      }
    }

    if (p.getFuncType() == FREE) {
      auto p_alloc = p.getNode<AllocateNode>()->ds;
      if (to_be_hoisted.count(p_alloc) > 0) {
        hoisted_free.push_back(p);
        pipeline[i] = new BlankNode();
      }
    }
  }

  util::printIterable(hoisted);
  util::printIterable(hoisted_free);

  // Now figure out which loops can be hoisted out
  // We can start to hoist out of loops until the first
  // parrallel loop is hit

  int outer_loop_index = outer_loops.size() - 1;
  int i = 0;
  for (i = outer_loop_index; i >= 0; i--) {
    if (outer_loops[i].var.isParallel()) {
      break;
    }
  }

  // add one back to i
  i++;

  std::vector<IntervalPipe> outer_loops_out;
  std::vector<IntervalPipe> outer_loops_in;

  for (int j = 0; j < outer_loops.size(); j++) {
    if (j >= i) {
      outer_loops_in.push_back(outer_loops[j]);
    } else {
      outer_loops_out.push_back(outer_loops[j]);
    }
  }

  Pipeline inner_pipeline = *this;
  inner_pipeline.outer_loops = outer_loops_in;

  Pipeline outer_pipeline;
  auto funcs = outer_pipeline.pipeline;
  funcs.insert(funcs.end(), hoisted.begin(), hoisted.end());
  funcs.push_back(new PipelineNode(inner_pipeline));
  funcs.insert(funcs.end(), hoisted_free.begin(), hoisted_free.end());
  outer_pipeline.pipeline = funcs;
  outer_pipeline.outer_loops = outer_loops_out;

  *this = outer_pipeline;

  // std::cout << outer_pipeline << std::endl;

  // util::printIterable(outer_loops_in);
  // util::printIterable(outer_loops_out);
}

} // namespace fern