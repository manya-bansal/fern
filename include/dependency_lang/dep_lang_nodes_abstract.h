#ifndef FERN_LANG_NODES_ABSTRACT
#define FERN_LANG_NODES_ABSTRACT

#include "type.h"
#include "utils/instrusive_ptr.h"

namespace fern {

class DependencyExprVisitorStrict;
class DependencySubsetVisitorStrict;
class DependencyIterVisitorStrict;

/// A node of a scalar index expression tree.
struct DependencyExprNode : public util::Manageable<DependencyExprNode>,
                            private util::Uncopyable {
public:
  DependencyExprNode() = default;
  DependencyExprNode(Datatype type);
  virtual ~DependencyExprNode() = default;

  virtual void accept(DependencyExprVisitorStrict *) const = 0;

  // /// Return the scalar data type of the index expression.
  Datatype getDataType() const;

private:
  Datatype dataType;
};

struct DependencySubsetNode : public util::Manageable<DependencySubsetNode>,
                              private util::Uncopyable {
public:
  DependencySubsetNode() = default;
  virtual ~DependencySubsetNode() = default;

  virtual void accept(DependencySubsetVisitorStrict *) const = 0;
};

struct DependencyIterNode : public util::Manageable<DependencyIterNode>,
                            private util::Uncopyable {
public:
  DependencyIterNode() = default;
  virtual ~DependencyIterNode() = default;

  virtual void accept(DependencyIterVisitorStrict *) const = 0;
};

} // namespace fern

#endif