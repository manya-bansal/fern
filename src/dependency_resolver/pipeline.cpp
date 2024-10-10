#include "dependency_resolver/pipeline.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "dependency_lang/dep_lang_rewriter.h"
#include "dependency_resolver/dependency_graph.h"
#include "utils/printer.h"
#include <algorithm>
#include <tuple>
#include <pybind11/pybind11.h>

namespace fern {
int add(int i, int j) {
    return i + j;
}

std::ostream &operator<<(std::ostream &os, const IntervalPipe &i) {
  os << i.var << " in (" << i.start << ", " << i.end << ", " << i.step << ")"
     << std::endl;
  return os;
}

// std::ostream &operator<<(std::ostream &os, const NodeMergeRel &rel) {

//   FERN_ASSERT_NO_MSG(v_rel == step_rel);
//   os << rel.v << " = ";
//   util::printIterable(rel.v_rel);
//   os << rel.step << " = ";

// }

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

  for (int i = last_func_index; i >= 0; i--) {
    std::map<const AbstractDataStructure *, std::string> names;
    auto call = functions[i];
    // std::cout << call << std::endl;
    std::vector<
        std::tuple<std::vector<DependencyExpr>, std::vector<DependencyExpr>>>
        dependencies;
    generateDependency(call, dependencies);

    for (auto a : call.getInputs()) {
      if (isIntermediate(a)) {
        // Skip because we are going to allocate it later
        names[a] = a->getVarName() + "_q";
        continue;
      }

      // Always query twice in the case that it's an input (no LCM node merging)
      std::string queried_name = util::uniqueName(a->getVarName() + "_q");

      // Otherwise query now
      auto deps = getCorrespondingDependency(call, a);

      if (deps.size() == 0) {
        names[a] = a->getVarName();
        continue;
      }

      queries.push_back(new QueryNode(a, deps, a->getVarName(), queried_name));
      // Add to free if required
      if (a->getQueryFreeInterface() != "__fern__not__defined__") {
        free.push_back(new FreeNode(a, queried_name, false));
      }

      names[a] = queried_name;
    }

    auto output = call.getOutput();
    std::string queried_name = output->getVarName() + "_q";
    auto deps = getCorrespondingDependency(call, output);
    if (isIntermediate(output)) {
      queries.push_back(new AllocateNode(output, deps, queried_name, call));
      if (output->getAllocFreeInterface() != "__fern__not__defined__") {
        free.push_back(new FreeNode(output, queried_name, true));
      }
    } else {
      queries.push_back(
          new QueryNode(output, deps, output->getVarName(), queried_name));
    }

    if (i == last_func_index) {
      if (output->insertQueried()) {
        compute.push_back(
            new InsertNode(output, deps, output->getVarName(), queried_name));
      }
    }

    names[output] = queried_name;
    compute.push_back(new ComputeNode(call, names, i));
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
              interval_vars.insert(getNode(v));
              ctx->match(op->child);
            }));

  // util::printIterable(outer_loops);
}

bool Pipeline::isIntermediate(const AbstractDataStructure *ds) const {
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
      var_relationships_sols.push_back(sol);
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
    os << get<0>(d) << " = " << get<1>(d) << std::endl;
  }

  for (auto rel : pipe.var_relationships_sols) {
    os << rel << std::endl;
  }

  for (auto funcs : pipe.pipeline) {
    os << '\t' << funcs << std::endl;
  }

  return os;
}

// Scheduling operators

static bool validSubpipeline(std::vector<ConcreteFunctionCall> calls,
                             int start_idx, int end_idx) {
  // Ensure that no intermediates in the subpipeline are being used later
  std::set<const AbstractDataStructure *> intermediates;

  // Skip the last one, it's the "final output that will be exposed"
  for (int i = start_idx; i < end_idx; i++) {
    intermediates.insert(calls[i].getOutput());
  }

  // Now start looking at the later functions and check if the
  // intermediates are being used as inputs
  // If yes, this is not a valid sub-pipeline
  for (int i = end_idx + 1; i < calls.size(); i++) {
    auto func = calls[i];
    auto inputs = func.getInputs();
    for (const auto &in : inputs) {
      if (intermediates.count(in)) {
        return false;
      }
    }
  }

  return true;
}

Pipeline Pipeline::subpipeline(int start_idx, int end_idx) {
  Pipeline new_pipe = *this;

  FERN_ASSERT_NO_MSG(start_idx >= 0);
  FERN_ASSERT_NO_MSG(start_idx <= end_idx);
  FERN_ASSERT_NO_MSG(end_idx < functions.size());
  FERN_ASSERT(validSubpipeline(new_pipe.functions, start_idx, end_idx),
              "Invalid Subpipeline Candidate");

  // Now that this is a valid subpipeline candidate, compute a new pipeline with
  // the nested subpipeline
  // Generate a new pipe but only of functions that compute until the
  // reuse ds.
  auto ds = new_pipe.functions[end_idx].getOutput();
  auto name = ds->getVarName() + "_q";
  std::vector<ConcreteFunctionCall> new_funcs;
  for (int i = start_idx; i <= end_idx; i++) {
    auto func = new_pipe.functions[i];
    auto concreteArgs = func.getArguments();
    concreteArgs[concreteArgs.size() - 1] =   Argument(std::make_shared<DataStructureArg>(
        DataStructurePtr(new MyConcreteDataStructure(ds, name))));
    auto new_func = ConcreteFunctionCall(func.getName(), concreteArgs,
                                         func.getOriginalDataRel(),
                                         func.getAbstractArguments());
    new_funcs.push_back(new_func);
    if (func.getOutput() == ds) {
      break;
    }
  }

  Pipeline p(new_funcs);
  p.constructPipeline();
  p = p.finalize();

  FunctionType subpipeline_node = new PipelineNode(p);
  // Set the previous computes to blank
  bool inserted = false;
  for (int i = 0; i < new_pipe.pipeline.size(); i++) {
    auto node = new_pipe.pipeline[i];
    if (node.getFuncType() == COMPUTE) {
      auto compute_func = node.getNode<ComputeNode>();
      if (compute_func->idx >= start_idx && compute_func->idx <= end_idx) {
        // Set this to blank
        new_pipe.pipeline[i] = new BlankNode();
        // Insert at the first available spot.
        if (!inserted) {
          new_pipe.pipeline[i] = subpipeline_node;
          inserted = true;
        }
      }
    }
  }

  return new_pipe;
}

static bool validBreak(std::vector<ConcreteFunctionCall> calls, int idx) {
  // Ensure that no intermediates in the subpipeline are being used later
  std::set<const AbstractDataStructure *> intermediates;

  // Skip the last one, it's the "final output that will be exposed"
  for (int i = 0; i < idx; i++) {
    intermediates.insert(calls[i].getOutput());
  }

  // Now start looking at the later functions and check if the
  // intermediates are being used as inputs
  // If yes, this is not a valid sub-pipeline
  for (int i = idx + 1; i < calls.size(); i++) {
    auto func = calls[i];
    auto inputs = func.getInputs();
    for (const auto &in : inputs) {
      if (intermediates.count(in)) {
        return false;
      }
    }
  }

  return true;
}

static void computeFullSteps(
    DependencySubset dataRelationship,
    std::vector<std::tuple<Variable, DependencyExpr>> &derivations) {

  match(dataRelationship,
        std::function<void(const IntervalNode *, Matcher *)>(
            [&](const IntervalNode *op, Matcher *ctx) {
              FERN_ASSERT(isa<Variable>(op->step),
                          "Cannot compute in Full Steps");
              derivations.push_back({op->var, 0});
              derivations.push_back({to<Variable>(op->step), op->end});

              ctx->match(op->child);
            }));
}

Pipeline Pipeline::breakPipeline(int idx) {
  FERN_ASSERT_NO_MSG(idx > 0);
  FERN_ASSERT_NO_MSG(idx < functions.size());
  FERN_ASSERT(validBreak(functions, idx), "Not a valid breaking point");

  // generate two sets of functions

  std::vector<ConcreteFunctionCall> pipe_0;
  std::vector<ConcreteFunctionCall> pipe_1;
  std::vector<ConcreteFunctionCall> pipe_1_mangled;

  for (int i = 0; i < idx; i++) {
    auto func = functions[i];
    pipe_0.push_back(func);
    pipe_1_mangled.push_back(ConcreteFunctionCall(
        func.getName(), func.getArguments(), func.getOriginalDataRel(),
        func.getAbstractArguments()));
  }

  for (int i = idx; i < functions.size(); i++) {
    auto func = functions[i];
    pipe_1.push_back(func);
    // Want to remangle the names
    pipe_1_mangled.push_back(ConcreteFunctionCall(
        func.getName(), func.getArguments(), func.getOriginalDataRel(),
        func.getAbstractArguments()));
  }

  // Allocate the temp. To do this, we will look at the var relationship
  // generated if the outer loops of the final output take full steps
  auto last_func = pipe_1_mangled[pipe_1_mangled.size() - 1];

  Pipeline p1_mangled(pipe_1_mangled);
  p1_mangled.constructPipeline();
  auto alloc_node =
      p1_mangled.getAllocateNode(pipe_1_mangled[pipe_0.size() - 1].getOutput());

  auto interval_vars = p1_mangled.getIntervalVars();
  for (auto v : interval_vars) {
    p1_mangled.derivations.push_back({Variable(v), 0});
  }
  // Will add to derivation by reference
  computeFullSteps(last_func.getDataRelationship(), p1_mangled.derivations);

  auto new_pipe = p1_mangled;

  new_pipe.pipeline = {};
  new_pipe.outer_loops = {};

  // Insert a new output alloc
  new_pipe.pipeline.push_back(new AllocateNode(alloc_node->ds, alloc_node->deps,
                                               alloc_node->ds->getVarName(),
                                               alloc_node->call));

  // Now let's add!
  Pipeline p0(pipe_0);
  p0.constructPipeline();
  p0 = p0.finalize();
  new_pipe.pipeline.push_back(new PipelineNode(p0));

  // Now let's add!
  Pipeline p1(pipe_1);
  p1.constructPipeline();
  p1 = p1.finalize();
  new_pipe.pipeline.push_back(new PipelineNode(p1));

  return new_pipe;
}

Pipeline Pipeline::reorder(int loop_1, int loop_2) {
  Pipeline new_pipe = *this;
  // Need to add a check for reorder
  // Make sure that you don't swap loops that depend on the values from
  // previous loops
  std::swap(new_pipe.outer_loops[loop_1], new_pipe.outer_loops[loop_2]);
  return new_pipe;
}

void Pipeline::runAutomaticIntermediateReuse() { automatic_reuse = true; }

Pipeline Pipeline::split(int loop, Variable outer, Variable inner,
                         Variable outer_step, Variable inner_step) {

  Pipeline new_pipe = *this;
  IntervalPipe old_loop = new_pipe.outer_loops[loop];

  // Cannot split if the loop step is not a pure variable
  FERN_ASSERT(isa<Variable>(old_loop.step),
              "Trying to split a loop where the step is a compound expr");

  Variable old_step = to<Variable>(old_loop.step);

  IntervalPipe outer_new(outer, old_loop.start, old_loop.end, outer_step);
  IntervalPipe inner_new(inner, outer, outer_step, inner_step);
  new_pipe.outer_loops[loop] = outer_new;
  new_pipe.outer_loops.insert(new_pipe.outer_loops.begin() + loop + 1,
                              inner_new);
  new_pipe.interval_vars.insert(getNode(outer));
  new_pipe.interval_vars.insert(getNode(inner));
  new_pipe.derivations.push_back(std::make_tuple(old_loop.var, outer + inner));
  new_pipe.derivations.push_back(std::make_tuple(old_step, inner_step));
  // new_pipe.derivations[old_step] = inner_step;
  new_pipe.derived_from[outer] = old_loop.var;
  new_pipe.derived_from[inner] = old_loop.var;
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
  FERN_ASSERT_NO_MSG(new_pipe.bounded_vars.count(var) == 0);
  new_pipe.bounded_vars[var] = val;
  return new_pipe;
}

Pipeline Pipeline::reuse(const AbstractDataStructure *ds, Variable v) {
  Pipeline new_pipe = *this;
  for (auto i = 0; i < new_pipe.pipeline.size(); i++) {
    auto p = new_pipe.pipeline[i];
    if (p.getFuncType() == ALLOCATE) {
      auto p_alloc = p.getNode<AllocateNode>();
      if (p_alloc->ds == ds) {
        new_pipe.pipeline[i] = new AllocateNode(
            p_alloc->ds, p_alloc->deps, p_alloc->name, p_alloc->call, true);
        new_pipe.to_reuse.push_back(ds);
        new_pipe.to_reuse_var.push_back(v);
        return new_pipe;
      }
    }
  }

  FERN_ASSERT(false, "Trying to reuse a data-structure that does not exist, "
                     "or is not an intermediate");
}

void Pipeline::register_resuable_allocs(
    std::map<const AbstractDataStructure *, const AbstractDataStructure *>
        reuse_intermediates) {
  reuse_intermediate_internal = reuse_intermediates;
};

Pipeline Pipeline::replaceDataStructure(const AbstractDataStructure *ds,
                                        std::string new_name) const {
  auto new_pipe = *this;
  for (int i = 0; i < new_pipe.pipeline.size(); i++) {
    auto func = pipeline[i];
    if (func.getFuncType() == ALLOCATE) {
      auto test = func.getNode<AllocateNode>();
      if (test->ds == ds) {
        new_pipe.pipeline[i] = new BlankNode();
      }
    }

    if (func.getFuncType() == COMPUTE) {
      auto test = func.getNode<ComputeNode>();
      std::map<const AbstractDataStructure *, std::string> new_names =
          test->names;

      for (auto name : test->names) {
        if (name.first == ds) {
          new_names[name.first] = new_name;
        }
      }
      new_pipe.pipeline[i] = new ComputeNode(test->func, new_names, i);
    }

    if (func.getFuncType() == FREE) {
      auto test = func.getNode<FreeNode>();
      if (test->ds == ds) {
        new_pipe.pipeline[i] = new BlankNode();
      }
    }

    if (func.getFuncType() == PIPELINE) {
      auto test = func.getNode<PipelineNode>();
      new_pipe.pipeline[i] =
          new PipelineNode(test->pipeline.replaceDataStructure(ds, new_name));
    }
  }

  return new_pipe;
}

Pipeline Pipeline::run_reuse_pass(
    std::map<const AbstractDataStructure *, const AbstractDataStructure *>
        reuse_intermediates) const {
  auto new_pipe = *this;

  for (auto pair : reuse_intermediates) {
    std::cout << *(pair.second) << std::endl;
    auto alloc_name = getAllocateNode(pair.second)->name;
    new_pipe = new_pipe.replaceDataStructure(pair.first, alloc_name);
  }
  return new_pipe;
}

Pipeline Pipeline::finalize(bool hoist) {

  auto new_pipe = *this;

  if (hoist) {
    new_pipe.run_hoisting_pass();
  }

  // Now figure out if any data-structures were marked as reuse.
  new_pipe = new_pipe.generate_reuse();

  if (!automatic_reuse) {
    new_pipe = new_pipe.run_reuse_pass(reuse_intermediate_internal);
  } else {
    auto reuse_substitutes = generate_reuse_substitutes();
    new_pipe = new_pipe.run_reuse_pass(reuse_substitutes);
  }

  return new_pipe;
}

static bool is_valid_reuse_candidate(const AbstractDataStructure *ds,
                                     FunctionType func) {

  FERN_ASSERT_NO_MSG(func.getNode<PipelineNode>());
  auto pipe_node = func.getNode<PipelineNode>();

  // Check 1: Cannot be reused if there is no loops underneath the alloc
  // So, it must be a pipeline

  // for all the pipeline nodes make this check
  for (auto f : pipe_node->pipeline.pipeline) {
    std::cout << f << std::endl;
    if (f.getFuncType() != PIPELINE) {
      continue;
    }
    auto f_node = f.getNode<PipelineNode>();
    std::cout << f_node->pipeline << std::endl;

    // Does this pipeline contain a compute on our data-structure?
    for (auto f_i : f_node->pipeline.pipeline) {
      if (f_i.getFuncType() != COMPUTE) {
        continue;
      }
      auto f_i_node = f_i.getNode<ComputeNode>();
      if (f_i_node->func.getOutput() == ds) {

        if (f_node->pipeline.outer_loops.size() == 1 &&
            !f_node->pipeline.outer_loops[0].var.isParallel()) {
          return true;
        }
      }
    }
  }

  return false;
}

static int get_child_pipeline_index(const AbstractDataStructure *ds,
                                    FunctionType func) {

  FERN_ASSERT_NO_MSG(func.getNode<PipelineNode>());
  auto pipe_node = func.getNode<PipelineNode>();

  // Check 1: Cannot be reused if there is no loops underneath the alloc
  // So, it must be a pipeline

  // for all the pipeline nodes make this check
  for (int i = 0; i < pipe_node->pipeline.pipeline.size(); i++) {
    auto f = pipe_node->pipeline.pipeline[i];
    if (f.getFuncType() != PIPELINE) {
      continue;
    }
    auto f_node = f.getNode<PipelineNode>();

    // Does this pipeline contain a compute on our data-structure?
    for (auto f_i : f_node->pipeline.pipeline) {
      if (f_i.getFuncType() != COMPUTE) {
        continue;
      }
      auto f_i_node = f_i.getNode<ComputeNode>();
      if (f_i_node->func.getOutput() == ds) {

        if (f_node->pipeline.outer_loops.size() == 1 &&
            !f_node->pipeline.outer_loops[0].var.isParallel()) {
          // std::cout << "Returning the child" << f_node->pipeline <<
          // std::endl;
          return i;
        }
      }
    }
  }

  // Should be unreached, should be calling is_valid_reuse_candidate
  // before calling this function!
  FERN_ASSERT_NO_MSG(false);
}

FunctionType
PipelineNode::get_host_pipeline(const AbstractDataStructure *ds) const {

  for (auto func : pipeline.pipeline) {
    if (func.getFuncType() == ALLOCATE) {
      auto test = func.getNode<AllocateNode>()->ds;
      if (test == ds) {
        return new PipelineNode(pipeline);
      }
    }

    if (func.getFuncType() == PIPELINE) {
      // If we have hit another pipeline, we have no allocas left, so we
      // will now recurse
      auto next_pipe = func.getNode<PipelineNode>();
      return next_pipe->get_host_pipeline(ds);
    }
  }

  // If we have not found anything, returned an undefined pipeline
  return FunctionType();
}

FunctionType Pipeline::getReusePreamble(const AbstractDataStructure *ds,
                                        std::string name) const {

  // Generate a new pipe but only of functions that compute uptil the
  // reuse ds.
  std::vector<ConcreteFunctionCall> new_funcs;
  for (auto func : functions) {
    auto concreteArgs = func.getArguments();
    concreteArgs[concreteArgs.size() - 1] =  Argument(std::make_shared<DataStructureArg>(
        DataStructurePtr(new MyConcreteDataStructure(ds, name))));
    auto new_func = ConcreteFunctionCall(func.getName(), concreteArgs,
                                         func.getOriginalDataRel(),
                                         func.getAbstractArguments());
    new_funcs.push_back(new_func);
    if (func.getOutput() == ds) {
      break;
    }
  }

  Pipeline p(new_funcs);
  p.constructPipeline();

  return new PipelineNode(p);
}

void Pipeline::place_new_host(FunctionType new_host,
                              const AbstractDataStructure *ds) {
  for (int i = 0; i < pipeline.size(); i++) {
    auto func = pipeline[i];
    if (func.getFuncType() == ALLOCATE) {
      auto test = func.getNode<AllocateNode>()->ds;
      if (test == ds) {
        pipeline[i] = new_host;
        return;
      }
    }

    if (func.getFuncType() == PIPELINE) {
      // If we have hit another pipeline, we have no allocas left, so we
      // will now recurse
      auto next_pipe = func.getNode<PipelineNode>();
      next_pipe->get_host_pipeline(ds);
    }
  }
}

Pipeline Pipeline::generate_reuse() {

  auto pipe_node = PipelineNode(*this);
  for (int i = 0; i < to_reuse.size(); i++) {
    auto reuse_ds = to_reuse[i];
    auto reuse_var = to_reuse_var[i];
    // First, we will look at the candidates for reuse
    // and locate it in pipeline where the data-structure is the parent.
    auto host_pipeline = pipe_node.get_host_pipeline(reuse_ds);

    if (host_pipeline.getFuncType() == UNDEFINED) {
      continue;
    }
    // Check whether this is a valid candidate for reuse in the first plade
    bool valid_resuse = is_valid_reuse_candidate(reuse_ds, host_pipeline);
    if (!valid_resuse) {
      FERN_ASSERT(false, "tried to mark an illegal data-structure as reusable");
    }
    std::cout << host_pipeline << std::endl;
    // Also get the index of the child, we will rewrite this to actually
    // perform the copy
    auto index = get_child_pipeline_index(reuse_ds, host_pipeline);

    // Generate the preamble for the "starting computation in the host
    // pipeline" Get premable for computing child
    auto preamble = getReusePreamble(reuse_ds, reuse_ds->getVarName() + "_q");

    auto host_pipeline_node = host_pipeline.getNode<PipelineNode>();

    auto new_host_pipe = host_pipeline_node->pipeline;
    new_host_pipe.pipeline.insert(new_host_pipe.pipeline.begin() + index,
                                  preamble);
    std::cout << new_host_pipe << std::endl;
    index++;

    // Now that our host pipeline is ready with the relevant start up, compute
    // what overlaps occur while computing the data-structure in the child
    // case.
    auto new_child = new_host_pipe.compute_valid_intersections(
        host_pipeline, new_host_pipe.pipeline[index], reuse_ds, reuse_var,
        reuse_ds->getVarName() + "_q");

    // Replace the old child now
    std::cout << new_child << std::endl;
    new_host_pipe.pipeline[index] = new_child;

    return new_host_pipe;
    // Place in the spot of the original host
    // pipe_node.pipeline.place_new_host(new PipelineNode(new_host_pipe),
    //                                   reuse_ds);
  }

  return pipe_node.pipeline;
}

const AllocateNode *
Pipeline::getAllocateNode(const AbstractDataStructure *ds) const {
  for (auto func : pipeline) {
    if (func.getFuncType() == ALLOCATE) {
      auto test = func.getNode<AllocateNode>();
      if (test->ds == ds) {
        return test;
      }
    }

    if (func.getFuncType() == PIPELINE) {
      auto test = func.getNode<PipelineNode>();
      return test->pipeline.getAllocateNode(ds);
    }
  }

  // Should never get here
  FERN_ASSERT_NO_MSG(false);
}

const QueryNode *Pipeline::getQueryNode(const AbstractDataStructure *ds) const {
  for (auto func : pipeline) {
    if (func.getFuncType() == QUERY) {
      auto test = func.getNode<QueryNode>();
      if (test->ds == ds) {
        return test;
      }
    }

    if (func.getFuncType() == PIPELINE) {
      auto test = func.getNode<PipelineNode>();
      return test->pipeline.getQueryNode(ds);
    }
  }

  // Should never get here
  FERN_ASSERT_NO_MSG(false);
}

int Pipeline::getQueryNodeIdx(std::string name) const {
  for (int i = 0; i < pipeline.size(); i++) {
    auto func = pipeline[i];
    if (func.getFuncType() == QUERY) {
      auto test = func.getNode<QueryNode>();
      if (test->child == name) {
        return i;
      }
    }

    if (func.getFuncType() == PIPELINE) {
      auto test = func.getNode<PipelineNode>();
      return test->pipeline.getQueryNodeIdx(name);
    }
  }

  // Should never get here
  FERN_ASSERT_NO_MSG(false);
}

const ComputeNode *
Pipeline::getComputeNode(const AbstractDataStructure *ds) const {
  for (auto func : pipeline) {
    if (func.getFuncType() == COMPUTE) {
      auto test = func.getNode<ComputeNode>();
      if (test->func.getOutput() == ds) {
        return test;
      }
    }

    if (func.getFuncType() == PIPELINE) {
      auto test = func.getNode<PipelineNode>();
      return test->pipeline.getComputeNode(ds);
    }
  }

  // Should never get here
  FERN_ASSERT_NO_MSG(false);
}

int Pipeline::getComputeNodeIdx(const AbstractDataStructure *ds) const {
  for (int i = 0; i < pipeline.size(); i++) {
    auto func = pipeline[i];
    if (func.getFuncType() == COMPUTE) {
      auto test = func.getNode<ComputeNode>();
      if (test->func.getOutput() == ds) {
        return i;
      }
    }

    if (func.getFuncType() == PIPELINE) {
      auto test = func.getNode<PipelineNode>();
      return test->pipeline.getComputeNodeIdx(ds);
    }
  }

  // Should never get here
  FERN_ASSERT_NO_MSG(false);
}

Variable Pipeline::getRootParent(Variable var) {
  if (derived_from.count(var) == 0) {
    return var;
  }

  return getRootParent(derived_from[var]);
}

static DependencyExpr generate_new_expr(DependencyExpr e,
                                        const DependencyVariableNode *v_org,
                                        const DependencyVariableNode *step,
                                        int shift_val, bool query = true) {

  struct ReplaceRewriter : public DependencyRewriter {
    using DependencyRewriter::visit;
    ReplaceRewriter(const DependencyVariableNode *v_org,
                    const DependencyVariableNode *step, int shift_val,
                    bool query)
        : v_org(v_org), step(step), shift_val(shift_val), query(query) {}

    void visit(const DependencyVariableNode *op) {
      if (op == v_org) {
        if (query) {
          expr = DependencyExpr(step) - shift_val;
        } else {
          expr = DependencyExpr(op) + DependencyExpr(step) - shift_val;
        }
      } else if (op == step) {
        expr = shift_val;

      } else {
        expr = DependencyExpr(op);
      }
    }

    const DependencyVariableNode *v_org;
    const DependencyVariableNode *step;
    int shift_val;
    bool query;
  };

  ReplaceRewriter rw(v_org, step, shift_val, query);
  return rw.rewrite(e);
}

static DependencyExpr generate_move_expr(DependencyExpr e,
                                         const DependencyVariableNode *v_org,
                                         const DependencyVariableNode *step,
                                         int start_val, int shift_val) {

  struct ReplaceRewriter : public DependencyRewriter {
    using DependencyRewriter::visit;
    ReplaceRewriter(const DependencyVariableNode *v_org,
                    const DependencyVariableNode *step, int start_val,
                    int shift_val)
        : v_org(v_org), step(step), start_val(start_val), shift_val(shift_val) {
    }

    void visit(const DependencyVariableNode *op) {
      if (op == v_org) {
        expr = start_val;
      } else if (op == step) {
        expr = DependencyExpr(step) - shift_val;
      } else {
        expr = DependencyExpr(op);
      }
    }

    const DependencyVariableNode *v_org;
    const DependencyVariableNode *step;
    int start_val;
    int shift_val;
  };

  ReplaceRewriter rw(v_org, step, start_val, shift_val);
  return rw.rewrite(e);
}

FunctionType
Pipeline::compute_valid_intersections(FunctionType parent, FunctionType child,
                                      const AbstractDataStructure *ds,
                                      Variable v, std::string name) {
  FERN_ASSERT(child.getFuncType() == PIPELINE,
              "How is child not a pipeline node? We need one loop at least!");
  auto p = child.getNode<PipelineNode>()->pipeline;
  auto p_host = parent.getNode<PipelineNode>()->pipeline;

  // Get the outer loop.
  auto o_loop = p.outer_loops[0];
  auto step_var = to<Variable>(o_loop.step);
  auto computeNodeIdx = p.getComputeNodeIdx(ds);
  auto computeNode = p.pipeline[computeNodeIdx].getNode<ComputeNode>();
  auto original_step =
      computeNode->func.getDataRelationship().get_interval_node(v)->step;
  FERN_ASSERT_NO_MSG(bounded_vars.count(step_var) > 0);

  std::vector<FunctionType> new_nodes;
  // Generate new output query Node for Output
  // get the allocation node from the host
  auto alloc_node_ds = p_host.getAllocateNode(ds);
  std::vector<DependencyExpr> deps_output_query;
  for (auto q : alloc_node_ds->deps) {
    deps_output_query.push_back(
        generate_new_expr(q, getNode(v), getNode(to<Variable>(original_step)),
                          bounded_vars[step_var], true));
  }

  auto output_q =
      FunctionType(new QueryNode(ds, deps_output_query, name, name + "_q"));
  new_nodes.push_back(output_q);

  // Now generate additional queries on all of the inputs for the compute func
  // We should already have larger queries ready.
  // Loop through the inputs of the compute nodes

  std::map<const AbstractDataStructure *, std::string> new_names;
  // COME BACK TO THIS
  for (auto old_names : computeNode->names) {
    if (old_names.first == ds) {
      new_names[old_names.first] = name + "_q";
    }

    else {
      // This must be an input, we need to generate a new query
      int query_node_idx = p.getQueryNodeIdx(old_names.second);
      auto input_q_node = p.pipeline[query_node_idx].getNode<QueryNode>();

      std::vector<DependencyExpr> input_q;
      for (auto q : input_q_node->deps) {
        input_q.push_back(generate_new_expr(
            q, getNode(v), getNode(to<Variable>(original_step)),
            bounded_vars[step_var], false));
      }
      auto i_q = FunctionType(new QueryNode(input_q_node->ds, input_q,
                                            input_q_node->ds->getVarName(),
                                            old_names.second));
      new_names[old_names.first] = old_names.second;
      // Erase the query node now.
      p.pipeline[query_node_idx] = new BlankNode();
      new_nodes.push_back(i_q);
    }
  }

  new_nodes.push_back(FunctionType(
      new ComputeNode(computeNode->func, new_names, computeNodeIdx)));

  // Erase the compute node
  p.pipeline[computeNodeIdx] = new BlankNode();

  // Now insert all ofo= our nodes
  for (int i = 0; i < new_nodes.size(); i++) {
    p.pipeline.insert(p.pipeline.begin() + i + computeNodeIdx, new_nodes[i]);
  }

  // Now generate the moving queries
  // This gets inserted at the VERY end.
  std::vector<DependencyExpr> move_query_src;
  for (auto q : alloc_node_ds->deps) {
    move_query_src.push_back(
        generate_move_expr(q, getNode(v), getNode(to<Variable>(original_step)),
                           bounded_vars[step_var], bounded_vars[step_var]));
  }
  auto output_move_src =
      FunctionType(new QueryNode(ds, move_query_src, name, name + "_move_src"));
  p.pipeline.push_back(output_move_src);

  std::vector<DependencyExpr> move_query_dst;
  for (auto q : alloc_node_ds->deps) {
    move_query_dst.push_back(
        generate_move_expr(q, getNode(v), getNode(to<Variable>(original_step)),
                           0, bounded_vars[step_var]));
  }
  auto output_move_insert = FunctionType(
      new InsertNode(ds, move_query_dst, name, name + "_move_src"));
  p.pipeline.push_back(output_move_insert);

  std::cout << p << std::endl;

  return FunctionType(new PipelineNode(p));
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

  std::vector<std::tuple<Variable, DependencyExpr>> outer_derivations;

  for (int j = 0; j < outer_loops.size(); j++) {
    if (j >= i) {
      outer_loops_in.push_back(outer_loops[j]);
      outer_derivations.push_back(std::make_tuple(outer_loops[j].var, 0));
      // If step is bound, add that too
      if (isa<Variable>(outer_loops[j].step)) {
        Variable step_bound = to<Variable>(outer_loops[j].step);
        if (step_bound.isBound()) {
          // outer_derivations[step_bound] = bounded_vars[step_bound];
          outer_derivations.push_back(
              std::make_tuple(step_bound, bounded_vars[step_bound]));
        }
      }

    } else {
      outer_loops_out.push_back(outer_loops[j]);
    }
  }

  Pipeline inner_pipeline = *this;
  inner_pipeline.outer_loops = outer_loops_in;

  outer_derivations.insert(outer_derivations.end(), derivations.begin(),
                           derivations.end());

  Pipeline outer_pipeline;
  auto funcs = outer_pipeline.pipeline;
  funcs.insert(funcs.end(), hoisted.begin(), hoisted.end());
  funcs.push_back(new PipelineNode(inner_pipeline));
  funcs.insert(funcs.end(), hoisted_free.begin(), hoisted_free.end());
  outer_pipeline.pipeline = funcs;
  outer_pipeline.outer_loops = outer_loops_out;
  outer_pipeline.to_reuse = to_reuse;
  outer_pipeline.var_relationships = var_relationships;
  outer_pipeline.var_relationships_sols = var_relationships_sols;
  outer_pipeline.bounded_vars = bounded_vars;
  outer_pipeline.defined = defined;
  outer_pipeline.undefined = undefined;
  outer_pipeline.dataflow_graph = dataflow_graph;
  outer_pipeline.computation_graph = computation_graph;
  outer_pipeline.functions = functions;
  outer_pipeline.derived_from = derived_from;
  outer_pipeline.to_reuse_var = to_reuse_var;
  outer_pipeline.derivations = outer_derivations;
  outer_pipeline.reuse_intermediate_internal = reuse_intermediate_internal;
  outer_pipeline.interval_vars = interval_vars;
  outer_pipeline.merge_rel_nodes = merge_rel_nodes;

  *this = outer_pipeline;
}

std::vector<const AbstractDataStructure *> Pipeline::getTrueInputs() const {
  std::vector<const AbstractDataStructure *> inputs;
  // to check if the input has already been to the return vector
  std::set<const AbstractDataStructure *> input_set;
  for (auto func : functions) {
    for (auto input : func.getInputs()) {
      if (!isIntermediate(input) && input != getFinalOutput()) {
        if (input_set.find(input) == input_set.end()) {
          inputs.push_back(input);
          input_set.insert(input);
        }
      }
    }
  }

  return inputs;
}

std::set<DummyDataStructure *> Pipeline::getDummyDatastructures() const {
  std::set<DummyDataStructure *> result;
  for (const auto &func : functions) {
    for (const auto &arg : func.getArguments()) {
      if (arg.getArgType() == DUMMY_DATASTRUCTURE) {
        auto ds = arg.getNode<DummyDataStructureArg>()->dsPtr();
        result.insert(ds);
      }
    }
  }
  return result;
}

const AbstractDataStructure *Pipeline::getFinalOutput() const {
  return functions[functions.size() - 1].getOutput();
}

std::set<const DependencyVariableNode *> Pipeline::getVariableArgs() const {
  std::set<const DependencyVariableNode *> result;
  for (const auto &func : functions) {
    for (const auto &arg : func.getArguments()) {
      if (arg.getArgType() == VARIABLE) {
        auto v = arg.getNode<VariableArg>()->getVariable();
        if (defined.count(v) <= 0) {
          result.insert(v);
        }
      }
    }
  }
  return result;
}

std::set<const DependencyVariableNode *> Pipeline::getIntervalVars() const {
  return interval_vars;
}

std::map<const AbstractDataStructure *, std::set<const AbstractDataStructure *>>
Pipeline::getAllIntermediateConflicts() const {

  std::map<const AbstractDataStructure *,
           std::set<const AbstractDataStructure *>>
      intermediateConflict;
  std::map<const AbstractDataStructure *, int> last_use_idx;
  std::map<const AbstractDataStructure *, int> first_use_index;

  // Figure out when the intermediate was last used as input
  for (int i = 0; i < functions.size(); i++) {
    for (auto in : functions[i].getInputs()) {
      if (isIntermediate(in)) {
        last_use_idx[in] = i;
        intermediateConflict[in] = {};
      }

      // Output should be assigned to once
      // Point at which the data-structure starts to contain reasonable values
      auto output = functions[i].getOutput();
      first_use_index[output] = i;
    }
  }

  // Create a set of conflicts
  for (auto ds : last_use_idx) {
    int final_func = ds.second;
    int start_func = first_use_index[ds.first];
    std::cout << *(ds.first) << " " << ds.second << std::endl;
    for (int i = start_func + 1; i <= final_func; i++) {
      auto f = functions[i];

      for (auto in : f.getInputs()) {
        if (in == ds.first) {
          continue;
        }
        if (isIntermediate(in)) {
          intermediateConflict[ds.first].insert(in);
        }
      }

      if (i != final_func) {
        auto out = f.getOutput();
        if (out == ds.first) {
          continue;
        }
        if (isIntermediate(out)) {
          intermediateConflict[ds.first].insert(out);
        }
      }
    }
  }

  return intermediateConflict;
}

std::vector<const AbstractDataStructure *>
Pipeline::getAllIntermediates() const {
  std::vector<const AbstractDataStructure *> intermediates;
  auto last_func_index = functions.size() - 1;
  // skip over the last function
  for (int i = 0; i < last_func_index; i++) {
    intermediates.push_back(functions[i].getOutput());
  }

  return intermediates;
}

std::map<const AbstractDataStructure *, const AbstractDataStructure *>
Pipeline::generate_reuse_substitutes() {

  std::map<const AbstractDataStructure *, const AbstractDataStructure *>
      substitutes;

  std::map<const AbstractDataStructure *,
           std::set<const AbstractDataStructure *>>
      share;

  auto conflicts = getAllIntermediateConflicts();
  auto intermediates = getAllIntermediates();

  // Loop through all the intermediates

  for (const auto &im : intermediates) {
    bool inserted = false;
    // Loop through all existing mappings
    for (auto mapping : share) {
      auto ds_conflicts = conflicts[mapping.first];
      if (ds_conflicts.count(im) > 0) {
        continue;
      }

      for (auto existing_mapping : mapping.second) {
        auto ds_conflicts = conflicts[existing_mapping];
        if (ds_conflicts.count(im) > 0) {
          continue;
        }
      }

      // DS does not conflict. Add to this entry and move on
      share[mapping.first].insert(im);
      inserted = true;
      break;
    }
    // If not inserted, start it's own entry
    if (!inserted)
      share[im] = {};
  }

  std::cout << std::endl;
  // For printing out !
  // for (auto c : share) {
  //   std::cout << *(c.first) << std::endl;
  //   std::cout << "********" << std::endl;

  //   for (auto d : c.second) {
  //     std::cout << *(d) << std::endl;
  //   }
  //   // util::printIterable(c.second);

  //   std::cout << std::endl;
  // }

  // Set up the values in substitutes!
  for (auto c : conflicts) {
    for (auto d : c.second) {
      substitutes[d] = c.first;
    }
  }

  // for (auto pair : substitutes) {
  //   std::cout << *(pair.first) << " is shared with " << *(pair.second)
  //             << std::endl;
  // }

  return substitutes;
}

static std::vector<FunctionType>
changeQueryToAlloc(std::vector<FunctionType> functions,
                   const AbstractDataStructure *ds, ConcreteFunctionCall call) {
  std::vector<FunctionType> to_return;
  for (auto f : functions) {
    if (f.getFuncType() == QUERY) {
      auto node = f.getNode<QueryNode>();
      if (node->ds == ds) {
        to_return.push_back(
            new AllocateNode(node->ds, node->deps, node->parent, call));
      } else {
        to_return.push_back(f);
      }

    } else if (f.getFuncType() == COMPUTE) {
      auto node = f.getNode<ComputeNode>();
      std::map<const AbstractDataStructure *, std::string> new_names;
      for (auto name : node->names) {
        if (name.first == ds) {
          new_names[name.first] = name.first->getVarName();
        } else {
          new_names[name.first] = name.second;
        }
      }
      to_return.push_back(new ComputeNode(node->func, new_names, node->idx));
    } else {
      to_return.push_back(f);
    }
  }
  return to_return;
}

int getIntervalVarIndex(DependencySubset dataRelationship, Variable v) {
  int index = -1;
  match(dataRelationship,
        std::function<void(const ProducerNode *, Matcher *)>(
            [&](const ProducerNode *op, Matcher *ctx) {
              FERN_ASSERT(isa<DataStructure>(op->child),
                          "Should always be a datastructure dep");
              auto ds = to<DataStructure>(op->child);
              auto deps = ds.getAnnotations();

              for (int i = 0; i < deps.size(); i++) {
                auto dep = deps[i];
                FERN_ASSERT(isa<Variable>(dep), "Should always be a var");
                auto dep_v = to<Variable>(dep);
                if (dep_v == v) {
                  index = i;
                }
              }
            }));

  return index;
}

static std::vector<std::vector<DependencyExpr>>
getAllQueryDeps(std::vector<FunctionType> functions,
                const AbstractDataStructure *ds) {
  std::vector<std::vector<DependencyExpr>> all_deps;

  for (auto f : functions) {
    if (f.getFuncType() == QUERY) {
      auto node = f.getNode<QueryNode>();
      if (node->ds == ds) {
        all_deps.push_back(node->deps);
      }
    }
  }
  return all_deps;
}

std::vector<NodeMergeRel>
generateDepRelationships(DependencySubset dataRelationship,
                         std::vector<std::vector<DependencyExpr>> all_deps) {

  std::vector<NodeMergeRel> rel_nodes;
  match(dataRelationship,
        std::function<void(const IntervalNode *, Matcher *)>(
            [&](const IntervalNode *op, Matcher *ctx) {
              int index_v = getIntervalVarIndex(dataRelationship, op->var);
              FERN_ASSERT_NO_MSG(index_v != -1);
              // std::cout << op->var << " = " << index_v << std::endl;
              FERN_ASSERT_NO_MSG(isa<Variable>(op->step));
              auto step_v = to<Variable>(op->step);
              int index_step = getIntervalVarIndex(dataRelationship, step_v);
              // std::cout << step_v << " = " << index_step << std::endl;

              std::vector<DependencyExpr> v_rel;
              std::vector<DependencyExpr> step_rel;
              for (auto deps : all_deps) {
                v_rel.push_back(deps[index_v]);
                step_rel.push_back(deps[index_step]);
              }

              rel_nodes.push_back(NodeMergeRel{
                  .v = op->var,
                  .step = step_v,
                  .v_rel = v_rel,
                  .step_rel = step_rel,
              });
            }));

  return rel_nodes;
}

void Pipeline::constructMergedPipeline() {

  constructPipeline();
  // First detect if we have a case where two functions request
  // output from the same function
  // To do so, build a graph and then query it!
  DependencyGraph graph(functions);
  auto fork_nodes = graph.identify_forks();
  FERN_ASSERT(fork_nodes.size() <= 1,
              "Cannot handle more than one fork node rn");

  // Loop from last call to first call (order is set up identify forks!)
  for (auto fork : fork_nodes) {
    // check if the children impose different constraints
    auto node_index = fork - 1;
    auto func = functions[node_index];
    auto children = graph.get_children(fork);
    bool merge = false;

    FERN_ASSERT(children.size() > 1,
                "Children size is less than 2, how is this a fork node?");
    // check for every pair, if there is one that breaks,
    // we will split
    for (int i = 0; i < children.size() - 1; i++) {
      auto func_1 = functions[children[i] - 1];
      auto func_2 = functions[children[i + 1] - 1];

      // Get the corresponding dependency of func_1
      auto constraints_1 = getCorrespondingDependency(func_1, func.getOutput());

      // Get the corresponding dependency of func_2
      auto constraints_2 = getCorrespondingDependency(func_2, func.getOutput());

      if (!check_abstract_equality(constraints_1, constraints_2)) {
        merge = true;
      }
    }

    if (merge) {
      // Now start the node merging

      // Generate a new pipeline (mangled) uptil the fork node
      std::vector<ConcreteFunctionCall> mangled_fork;

      for (int i = 0; i <= node_index; i++) {
        auto f = functions[i];
        mangled_fork.push_back(ConcreteFunctionCall(
            f.getName(), f.getArguments(), f.getOriginalDataRel(),
            f.getAbstractArguments()));
      }

      // Generate a normal pipeline after the fork node (no need to mangle
      // here)
      std::vector<ConcreteFunctionCall> after_fork;
      for (int i = node_index + 1; i < functions.size(); i++) {
        auto f = functions[i];
        after_fork.push_back(f);
      }

      // Construct a pipeline with the mangled nodes
      Pipeline b_fork(mangled_fork);
      b_fork.constructPipeline();
      b_fork.pipeline =
          changeQueryToAlloc(b_fork.pipeline, func.getOutput(), func);
      std::cout << b_fork << std::endl;

      // Construct a pipeline with the normal nodes
      Pipeline a_fork(after_fork);
      a_fork.constructPipeline();

      // Add the constraints for the new vars

      // Get the meta data values used in the Query Deps
      auto allQueryDeps = getAllQueryDeps(a_fork.pipeline, func.getOutput());

      // Loop through each interval var in b_fork data rel and generate the
      // equations
      auto rel_nodes = generateDepRelationships(
          b_fork.functions[b_fork.functions.size() - 1].getDataRelationship(),
          allQueryDeps);

      a_fork.var_relationships_sols.insert(
          a_fork.var_relationships_sols.begin(),
          b_fork.var_relationships_sols.begin(),
          b_fork.var_relationships_sols.end());
      a_fork.pipeline.insert(a_fork.pipeline.begin(), b_fork.pipeline.begin(),
                             b_fork.pipeline.end());
      a_fork.merge_rel_nodes = rel_nodes;

      *this = a_fork;
    }
  }
}

} // namespace fern

PYBIND11_MODULE(fern_py, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring

    m.def("add", &fern::add, "A function that adds two numbers");
}