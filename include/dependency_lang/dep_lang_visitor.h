#ifndef DEPENDENCY_LANG_VISITOR
#define DEPENDENCY_LANG_VISITOR

#include <cassert>
#include <functional>

namespace fern {

class DependencyExpr;
struct DependencyLiteralNode;
struct DependencyVariableNode;
// struct DependencyArgumentNode;
struct AddNode;
struct SubNode;
struct MulNode;
struct DivNode;
struct ModNode;
struct MetaDataNode;

class DependencySubset;
// struct SubsetNode;
struct DataStructureNode;
struct ProducerNode;
struct ConsumerNode;
struct ComputationAnnotationNode;

class DependencyIter;
struct IntervalNode;

class DependencyExprVisitorStrict {
public:
  virtual ~DependencyExprVisitorStrict() = default;

  void visit(const DependencyExpr &);

  virtual void visit(const DependencyLiteralNode *) = 0;
  virtual void visit(const DependencyVariableNode *) = 0;
  // virtual void visit(const DependencyArgumentNode*) = 0;
  virtual void visit(const AddNode *) = 0;
  virtual void visit(const SubNode *) = 0;
  virtual void visit(const DivNode *) = 0;
  virtual void visit(const MulNode *) = 0;
  virtual void visit(const ModNode *) = 0;
  virtual void visit(const MetaDataNode *) = 0;
};

class DependencySubsetVisitorStrict {
public:
  virtual ~DependencySubsetVisitorStrict() = default;

  void visit(const DependencySubset &);

  // virtual void visit(const SubsetNode*) = 0;
  virtual void visit(const DataStructureNode *) = 0;
  virtual void visit(const IntervalNode *) = 0;
  virtual void visit(const ProducerNode *) = 0;
  virtual void visit(const ConsumerNode *) = 0;
  virtual void visit(const ComputationAnnotationNode *) = 0;
};

class DependencyIterVisitorStrict {
public:
  virtual ~DependencyIterVisitorStrict() = default;

  void visit(const DependencyIter &);
};

/// Visit nodes in index notation
class DependencyVisitorStrict : public DependencyExprVisitorStrict,
                                public DependencySubsetVisitorStrict,
                                public DependencyIterVisitorStrict {
public:
  virtual ~DependencyVisitorStrict() = default;

  using DependencyExprVisitorStrict::visit;
  using DependencyIterVisitorStrict::visit;
  using DependencySubsetVisitorStrict::visit;
};

class DependencyVisitor : public DependencyVisitorStrict {
public:
  virtual ~DependencyVisitor() = default;

  using DependencyVisitorStrict::visit;

  virtual void visit(const DependencyLiteralNode *);
  virtual void visit(const DependencyVariableNode *);
  // virtual void visit(const DependencyArgumentNode*);
  virtual void visit(const AddNode *);
  virtual void visit(const SubNode *);
  virtual void visit(const DivNode *);
  virtual void visit(const MulNode *);
  virtual void visit(const ModNode *);
  virtual void visit(const MetaDataNode *);

  // virtual void visit(const SubsetNode*);
  virtual void visit(const DataStructureNode *);
  virtual void visit(const ProducerNode *);
  virtual void visit(const ConsumerNode *);
  virtual void visit(const ComputationAnnotationNode *);

  virtual void visit(const IntervalNode *);
};

#define RULE(Rule)                                                             \
  std::function<void(const Rule *)> Rule##Func;                                \
  std::function<void(const Rule *, Matcher *)> Rule##CtxFunc;                  \
  void unpack(std::function<void(const Rule *)> pattern) {                     \
    assert(!Rule##CtxFunc && !Rule##Func);                                     \
    Rule##Func = pattern;                                                      \
  }                                                                            \
  void unpack(std::function<void(const Rule *, Matcher *)> pattern) {          \
    assert(!Rule##CtxFunc && !Rule##Func);                                     \
    Rule##CtxFunc = pattern;                                                   \
  }                                                                            \
  void visit(const Rule *op) {                                                 \
    if (Rule##Func) {                                                          \
      Rule##Func(op);                                                          \
    } else if (Rule##CtxFunc) {                                                \
      Rule##CtxFunc(op, this);                                                 \
      return;                                                                  \
    }                                                                          \
    DependencyVisitor::visit(op);                                              \
  }

class Matcher : public DependencyVisitor {
public:
  template <class Stmt> void match(Stmt stmt) { stmt.accept(this); }

  template <class IR, class... Patterns>
  void process(IR ir, Patterns... patterns) {
    unpack(patterns...);
    ir.accept(this);
  }

private:
  template <class First, class... Rest> void unpack(First first, Rest... rest) {
    unpack(first);
    unpack(rest...);
  }

  using DependencyVisitor::visit;

  RULE(DependencyLiteralNode);
  RULE(DependencyVariableNode);
  RULE(AddNode);
  RULE(SubNode);
  RULE(MulNode);
  RULE(DivNode);
  RULE(ModNode);
  RULE(MetaDataNode);

  // RULE(SubsetNode);
  RULE(DataStructureNode);
  RULE(IntervalNode);
  RULE(ProducerNode);
  RULE(ConsumerNode);
  RULE(ComputationAnnotationNode);
};

/**
  Match patterns to the IR.  Use lambda closures to capture environment
  variables (e.g. [&]):

  For example, to print all AddNode and SubNode objects in func:
  ~~~~~~~~~~~~~~~{.cpp}
  match(expr,
    std::function<void(const AddNode*)>([](const AddNode* op) {
      // ...
    })
    ,
    std::function<void(const SubNode*)>([](const SubNode* op) {
      // ...
    })
  );
  ~~~~~~~~~~~~~~~

  Alternatively, mathing rules can also accept a Context to be used to match
  sub-expressions:
  ~~~~~~~~~~~~~~~{.cpp}
  match(expr,
    std::function<void(const SubNode*,Matcher*)>([&](const SubNode* op,
                                                     Matcher* ctx){
      ctx->match(op->a);
    })
  );
  ~~~~~~~~~~~~~~~

  function<void(const Add*, Matcher* ctx)>([&](const Add* op, Matcher* ctx) {
**/
template <class Stmt, class... Patterns>
void match(Stmt stmt, Patterns... patterns) {
  if (!stmt.defined()) {
    return;
  }
  Matcher().process(stmt, patterns...);
}

} // namespace fern

#endif