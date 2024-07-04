#ifndef PIPELINE_ABSTRACT_NODES
#define PIPELINE_ABSTRACT_NODES

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

public:
  Pipeline() = default;
  Pipeline(std::vector<ConcreteFunctionCall> functions)
      : functions(functions) {}
  virtual ~Pipeline() = default;

  std::vector<ConcreteFunctionCall> functions;
  std::vector<FunctionType> pipeline;
};

} // namespace fern

#endif