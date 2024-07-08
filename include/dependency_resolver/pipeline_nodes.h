#ifndef PIPELINE_ABSTRACT_NODES
#define PIPELINE_ABSTRACT_NODES

#include <ginac/ginac.h>

#include "dependency_lang/dep_lang.h"
#include "dependency_resolver/dependency_solver.h"
#include "fern_error.h"
#include "utils/instrusive_ptr.h"

#include <map>
#include <ostream>
#include <stack>
#include <vector>

namespace fern {

struct FunctionTypeNode;
struct Pipeline;

enum Action {
  UNDEFINED,
  COMPUTE,
  QUERY,
  ALLOCATE,
  INSERT,
  FREE,
  PIPELINE,
  BLANK
};
// A pointer class for FunctionType provides some operations
// for all FunctionTypes
class FunctionType : public util::IntrusivePtr<const FunctionTypeNode> {
public:
  FunctionType() : FunctionType(nullptr) {}
  FunctionType(FunctionTypeNode *node) : IntrusivePtr(node) {}
  void print(std::ostream &stream) const;
  // bool equals(const IndexVarRel &rel) const;
  Action getFuncType() const;

  template <typename T> const T *getNode() const {
    return static_cast<const T *>(ptr);
  }

  const FunctionTypeNode *getNode() const { return ptr; }
};

std::ostream &operator<<(std::ostream &, const FunctionType &);

struct FunctionTypeNode : public util::Manageable<FunctionTypeNode>,
                          private util::Uncopyable {
  FunctionTypeNode() : action(UNDEFINED) {}
  FunctionTypeNode(Action type) : action(type) {}
  virtual ~FunctionTypeNode() = default;
  virtual void print(std::ostream &stream) const;

  Action action;
};

struct QueryNode : public FunctionTypeNode {

  QueryNode(const AbstractDataStructure *ds, std::vector<DependencyExpr> deps,
            const std::string &parent, const std::string &child)
      : FunctionTypeNode(QUERY), ds(ds), deps(deps), parent(parent),
        child(child) {}

  const AbstractDataStructure *ds;
  std::vector<DependencyExpr> deps;

  void print(std::ostream &stream) const override;

  std::string parent;
  std::string child;
};

struct AllocateNode : public FunctionTypeNode {

  AllocateNode(const AbstractDataStructure *ds,
               std::vector<DependencyExpr> deps, std::string name,
               ConcreteFunctionCall call, bool reuse = false)
      : FunctionTypeNode(ALLOCATE), ds(ds), deps(deps), name(name), call(call),
        reuse(reuse) {}

  void print(std::ostream &stream) const override;
  void set_reuse();
  const AbstractDataStructure *ds;
  std::vector<DependencyExpr> deps;
  std::string name;
  ConcreteFunctionCall call;
  bool reuse = false;
};

struct InsertNode : public FunctionTypeNode {

  InsertNode(const AbstractDataStructure *ds, std::vector<DependencyExpr> deps,
             const std::string &parent, const std::string &child)
      : FunctionTypeNode(INSERT), ds(ds), deps(deps), parent(parent),
        child(child) {}

  void print(std::ostream &stream) const override;
  const AbstractDataStructure *ds;
  std::vector<DependencyExpr> deps;
  std::string parent;
  std::string child;
};

struct FreeNode : public FunctionTypeNode {
  FreeNode(const AbstractDataStructure *ds, const std::string &name,
           bool allocate)
      : FunctionTypeNode(FREE), ds(ds), name(name), allocate(allocate) {}

  void print(std::ostream &stream) const override;
  const AbstractDataStructure *ds;
  std::string name;
  bool allocate;
};

struct ComputeNode : public FunctionTypeNode {

  ComputeNode(ConcreteFunctionCall func,
              std::map<const AbstractDataStructure *, std::string> names)
      : FunctionTypeNode(COMPUTE), func(func), names(names) {}

  void print(std::ostream &stream) const override;
  ConcreteFunctionCall func;
  std::map<const AbstractDataStructure *, std::string> names;
};

// Filler to manipulate objects (like vectors etc)
// while iterating over them
struct BlankNode : public FunctionTypeNode {
  BlankNode() : FunctionTypeNode(BLANK){};
  void print(std::ostream &stream) const override;
};

struct IntervalPipe {

  IntervalPipe(Variable var, DependencyExpr start, DependencyExpr end,
               DependencyExpr step)
      : var(var), start(start), end(end), step(step) {}

  Variable var;
  DependencyExpr start;
  DependencyExpr end;
  DependencyExpr step;
};

std::ostream &operator<<(std::ostream &, const IntervalPipe &);

struct Pipeline {
  Pipeline() = default;
  Pipeline(std::vector<ConcreteFunctionCall> functions) : functions(functions) {
    FERN_ASSERT(functions.size() > 0, "Pipeline length should at least be 1");
  }

  void constructPipeline();
  void buildDataFlowGraph();
  void buildComputationGraph();

  void buildNaivePipeline();
  void buildFuncCalls();
  void generateOuterLoops();

  std::vector<const AbstractDataStructure *> getTrueInputs() const;
  std::set<DummyDataStructure *> getDummyDatastructures() const;
  const AbstractDataStructure *getFinalOutput() const;
  std::set<const DependencyVariableNode *> getVariableArgs() const;
  std::set<const DependencyVariableNode *> getIntervalVars() const;

  Pipeline finalize(bool hoist = true);
  void run_hoisting_pass();
  bool hoist_able(const AllocateNode *node);
  Pipeline generate_reuse();
  Variable getRootParent(Variable var);
  FunctionType getReusePreamble(const AbstractDataStructure *ds,
                                std::string name) const;

  const AbstractDataStructure *
  corresponding_abstract_var(ConcreteFunctionCall call,
                             const AbstractDataStructure *ds) const;

  const AllocateNode *getAllocateNode(const AbstractDataStructure *ds) const;
  int getComputeNodeIdx(const AbstractDataStructure *ds) const;
  const ComputeNode *getComputeNode(const AbstractDataStructure *ds) const;
  int getQueryNodeIdx(std::string name) const;
  const QueryNode *getQueryNode(const AbstractDataStructure *ds) const;
  void place_new_host(FunctionType new_host, const AbstractDataStructure *ds);
  void generateDependency(
      ConcreteFunctionCall call,
      std::vector<std::tuple<std::vector<DependencyExpr>,
                             std::vector<DependencyExpr>>> &dependencies);

  std::vector<DependencyExpr>
  getCorrespondingDependency(ConcreteFunctionCall call,
                             const AbstractDataStructure *ds,
                             int arg_place = -1) const;

  FunctionType compute_valid_intersections(FunctionType parent,
                                           FunctionType pipeline,
                                           const AbstractDataStructure *ds,
                                           Variable v, std::string name);

  bool isIntermediate(const AbstractDataStructure *ds) const;

  Pipeline reorder(int loop_1, int loop_2);
  Pipeline split(int loop, Variable outer, Variable inner, Variable outer_step,
                 Variable inner_step);
  Pipeline parrallelize(int loop);
  Pipeline bind(Variable var, int val);
  Pipeline reuse(const AbstractDataStructure *ds, Variable v);

  void register_resuable_allocs(
      std::map<const AbstractDataStructure *, const AbstractDataStructure *>
          reuse_intermediates);
  Pipeline run_reuse_pass(
      std::map<const AbstractDataStructure *, const AbstractDataStructure *>
          reuse_intermediates) const;
  Pipeline replaceDataStructure(const AbstractDataStructure *ds,
                                std::string new_name) const;

  std::vector<ConcreteFunctionCall> functions;

  std::vector<FunctionType> pipeline;
  std::vector<std::tuple<const AbstractDataStructure *,
                         std::set<const AbstractDataStructure *>>>
      dataflow_graph;

  std::map<const AbstractDataStructure *, ConcreteFunctionCall>
      computation_graph;

  std::map<const DependencyVariableNode *, GiNaC::ex> var_relationships;
  std::vector<solution> var_relationships_sols;
  std::set<const DependencyVariableNode *> undefined;
  std::set<const DependencyVariableNode *> defined;
  std::vector<const AbstractDataStructure *> to_reuse;
  std::vector<Variable> to_reuse_var;

  // The outer loops to wrap the output in
  std::vector<IntervalPipe> outer_loops;
  std::map<Variable, int> bounded_vars;
  std::vector<std::tuple<Variable, DependencyExpr>> derivations;
  std::map<Variable, Variable> derived_from;
  std::map<const AbstractDataStructure *, const AbstractDataStructure *>
      reuse_intermediate_internal;
  std::set<const DependencyVariableNode *> interval_vars;
};

std::ostream &operator<<(std::ostream &, const Pipeline &);

struct PipelineNode : public FunctionTypeNode {
  PipelineNode(Pipeline pipeline)
      : FunctionTypeNode(PIPELINE), pipeline(pipeline) {}
  void print(std::ostream &stream) const override;
  FunctionType get_host_pipeline(const AbstractDataStructure *ds) const;
  Pipeline pipeline;
};

} // namespace fern

#endif