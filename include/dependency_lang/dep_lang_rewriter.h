#ifndef DEPENDENCY_LANG_REWRITER
#define DEPENDENCY_LANG_REWRITER

#include <map>

#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_visitor.h"

namespace fern {

class DependencyExprRewriterStrict : public DependencyExprVisitorStrict {
public:
  virtual ~DependencyExprRewriterStrict() {}

  DependencyExpr rewrite(DependencyExpr);

protected:
  DependencyExpr expr;

  using DependencyExprVisitorStrict::visit;

  virtual void visit(const DependencyLiteralNode *) = 0;
  virtual void visit(const DependencyVariableNode *) = 0;
  // virtual void visit(const DependencyArgumentNode*) = 0;
  virtual void visit(const AddNode *) = 0;
  virtual void visit(const SubNode *) = 0;
  virtual void visit(const DivNode *) = 0;
  virtual void visit(const MulNode *) = 0;
  virtual void visit(const ModNode *) = 0;
};

class DependencySubsetRewriterStrict : public DependencySubsetVisitorStrict {
public:
  virtual ~DependencySubsetRewriterStrict() {}

  DependencySubset rewrite(DependencySubset);

protected:
  /// Assign to stmt in visit methods to replace the visited stmt.
  DependencySubset subset;

  using DependencySubsetVisitorStrict::visit;

  // virtual void visit(const SubsetNode*) = 0;
  virtual void visit(const DataStructureNode *) = 0;
  virtual void visit(const IntervalNode *) = 0;
  virtual void visit(const ProducerNode *) = 0;
  virtual void visit(const ConsumerNode *) = 0;
  virtual void visit(const ComputationAnnotationNode *) = 0;
};

class DependencyRewriterStrict : public DependencyExprRewriterStrict,
                                 public DependencySubsetRewriterStrict {
public:
  virtual ~DependencyRewriterStrict() {}

  using DependencyExprRewriterStrict::rewrite;
  using DependencySubsetRewriterStrict::rewrite;

protected:
  using DependencyExprVisitorStrict::visit;
  using DependencySubsetVisitorStrict::visit;
};

class DependencyRewriter : public DependencyRewriterStrict {
public:
  virtual ~DependencyRewriter() {}

protected:
  using DependencyRewriterStrict::visit;

  virtual void visit(const DependencyLiteralNode *);
  virtual void visit(const DependencyVariableNode *);
  // virtual void visit(const DependencyArgumentNode*) = 0;
  virtual void visit(const AddNode *);
  virtual void visit(const SubNode *);
  virtual void visit(const DivNode *);
  virtual void visit(const MulNode *);
  virtual void visit(const ModNode *);
  virtual void visit(const MetaDataNode *);

  // virtual void visit(const SubsetNode*);
  virtual void visit(const DataStructureNode *);
  virtual void visit(const IntervalNode *);
  virtual void visit(const ProducerNode *);
  virtual void visit(const ConsumerNode *);
  virtual void visit(const ComputationAnnotationNode *);
};

DependencySubset ChangeVariableNames(
    DependencySubset s,
    std::map<const DependencyVariableNode *, const DependencyVariableNode *>
        replace);

class FlattenExpr : public DependencyExprVisitorStrict {
public:
  using DependencyExprVisitorStrict::visit;
  enum node_type {
    LITERAL,
    VAR,
    ADD,
    SUB,
    DIV,
    MOD,
    MUL,
    META_DATA,
  };

  void visit(const DependencyLiteralNode *);
  void visit(const DependencyVariableNode *);
  void visit(const AddNode *);
  void visit(const SubNode *);
  void visit(const DivNode *);
  void visit(const MulNode *);
  void visit(const ModNode *);
  void visit(const MetaDataNode *);

  std::vector<node_type> nodeTypes;
  std::vector<DependencyExpr> nodes;
};

bool check_abstract_equality(std::vector<DependencyExpr> e1,
                             std::vector<DependencyExpr> e2);

} // namespace fern

#endif