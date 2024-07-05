#ifndef PIPELINE_ABSTRACT_NODES
#define PIPELINE_ABSTRACT_NODES

#include <ginac/ginac.h>

#include "dependency_lang/dep_lang.h"
#include "dependency_resolver/dependency_solver.h"
#include "fern_error.h"
#include "utils/instrusive_ptr.h"

#include <map>
#include <ostream>
#include <vector>

namespace fern {

struct FunctionTypeNode;
struct Pipeline;

enum Action { UNDEFINED, COMPUTE, QUERY, ALLOCATE, INSERT, FREE, PIPELINE };
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
               std::vector<DependencyExpr> deps, std::string name)
      : FunctionTypeNode(ALLOCATE), ds(ds), deps(deps), name(name) {}

  void print(std::ostream &stream) const override;
  const AbstractDataStructure *ds;
  std::vector<DependencyExpr> deps;
  std::string name;
};

struct InsertNode : public FunctionTypeNode {

  InsertNode(const AbstractDataStructure *ds, const std::string &parent,
             const std::string &child)
      : FunctionTypeNode(INSERT), ds(ds), parent(parent), child(child) {}

  void print(std::ostream &stream) const override;
  const AbstractDataStructure *ds;
  std::string parent;
  std::string child;
};

struct FreeNode : public FunctionTypeNode {
  FreeNode(const AbstractDataStructure *ds, const std::string &name)
      : FunctionTypeNode(FREE), ds(ds), name(name) {}

  void print(std::ostream &stream) const override;
  const AbstractDataStructure *ds;
  std::string name;
};

struct ComputeNode : public FunctionTypeNode {

  ComputeNode(ConcreteFunctionCall func,
              std::map<const AbstractDataStructure *, std::string> names)
      : FunctionTypeNode(COMPUTE), func(func), names(names) {}

  void print(std::ostream &stream) const override;
  ConcreteFunctionCall func;
  std::map<const AbstractDataStructure *, std::string> names;
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

  std::vector<DependencyExpr>
  getCorrespondingConstraint(ConcreteFunctionCall call,
                             const AbstractDataStructure *ds,
                             int arg_place = -1) const;

  const AbstractDataStructure *
  corresponding_abstract_var(ConcreteFunctionCall call,
                             const AbstractDataStructure *ds) const;

  void generateDependency(
      ConcreteFunctionCall call,
      std::vector<std::tuple<std::vector<DependencyExpr>,
                             std::vector<DependencyExpr>>> &dependencies);

  std::vector<DependencyExpr>
  getCorrespondingDependency(ConcreteFunctionCall call,
                             const AbstractDataStructure *ds,
                             int arg_place = -1) const;

  bool isIntermediate(const AbstractDataStructure *ds);

  std::vector<ConcreteFunctionCall> functions;

  std::vector<FunctionType> pipeline;
  std::vector<std::tuple<const AbstractDataStructure *,
                         std::set<const AbstractDataStructure *>>>
      dataflow_graph;

  std::map<const AbstractDataStructure *, ConcreteFunctionCall>
      computation_graph;

  std::map<const DependencyVariableNode *, GiNaC::ex> var_relationships;
  std::set<const DependencyVariableNode *> undefined;
  std::set<const DependencyVariableNode *> defined;

  // The outer loops to wrap the output in
  std::vector<IntervalPipe> outer_loops;
};

std::ostream &operator<<(std::ostream &, const Pipeline &);

struct PipelineNode : public FunctionTypeNode {
  PipelineNode(Pipeline pipeline) : pipeline(pipeline) {}
  Pipeline pipeline;
};

} // namespace fern

#endif