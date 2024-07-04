#ifndef PIPELINE_ABSTRACT_NODES
#define PIPELINE_ABSTRACT_NODES

#include <ginac/ginac.h>

#include "dependency_lang/dep_lang.h"
#include "fern_error.h"
#include "utils/instrusive_ptr.h"

#include <ostream>
#include <vector>

namespace fern {

struct FunctionTypeNode;

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

struct FunctionTypeNode : public util::Manageable<FunctionTypeNode>,
                          private util::Uncopyable {
  FunctionTypeNode() : action(UNDEFINED) {}
  FunctionTypeNode(Action type) : action(type) {}
  virtual ~FunctionTypeNode() = default;
  virtual void print(std::ostream &stream) const {
    FERN_ASSERT_NO_MSG(action == UNDEFINED);
    stream << "underived";
  }

  Action action;
};

struct Pipeline {
  Pipeline() = default;
  Pipeline(std::vector<ConcreteFunctionCall> functions)
      : functions(functions) {}
  virtual ~Pipeline() = default;

  std::vector<ConcreteFunctionCall> functions;
  std::vector<FunctionType> pipeline;
};

struct QueryNode : public FunctionTypeNode {

  QueryNode(const AbstractDataStructure *ds,
            std::vector<solution> meta_data_vals, const std::string &parent,
            const std::string &child)
      : FunctionTypeNode(QUERY), ds(ds), meta_data_vals(meta_data_vals),
        parent(parent), child(child) {}

  const AbstractDataStructure *ds;
  std::vector<solution> meta_data_vals;
  std::string parent;
  std::string child;
};

struct AllocateNode : public FunctionTypeNode {

  AllocateNode(const AbstractDataStructure *ds,
               std::vector<solution> meta_data_vals, std::string name)
      : FunctionTypeNode(ALLOCATE), ds(ds), meta_data_vals(meta_data_vals),
        name(name) {}

  const AbstractDataStructure *ds;
  std::vector<solution> meta_data_vals;
  std::string name;
};

struct InsertNode : public FunctionTypeNode {

  InsertNode(const AbstractDataStructure *ds, sonst std::string &parent,
             const std::string &child)
      : FunctionTypeNode(INSERT), ds(ds), parent(parent), child(child) {}

  const AbstractDataStructure *ds;
  std::string parent;
  std::string child;
};

struct FreeNode : public FunctionTypeNode {
  FreeNode(const AbstractDataStructure *ds, sonst std::string &name)
      : FunctionTypeNode(FREE), ds(ds), name(name) {}

  const AbstractDataStructure *ds;
  std::string name;
};

struct ComputeNode : public FunctionTypeNode {

  ComputeNode(ConcreteFunctionCall func)
      : FunctionTypeNode(COMPUTE), func(func) {}

  ConcreteFunctionCall func;
};

struct PipelineNode : public FunctionTypeNode {
  PipelineNode(Pipeline pipeline) : pipeline(pipeline) {}
  Pipeline pipeline;
};

} // namespace fern

#endif