#ifndef DEPENDENCY_LANG_PRINTER
#define DEPENDENCY_LANG_PRINTER

#include <ostream>

#include "dependency_lang/dep_lang_visitor.h"

namespace fern {

class DependencyPrinter : public DependencyVisitorStrict {
public:
  DependencyPrinter(std::ostream &os);
  // DependencyPrinter(std::ostream& os, int ident_level);

  void print(const DependencyExpr &);
  void print(const DependencySubset &);
  void print(const DependencyIter &);
  // void print(const AcceleratorStmt& expr);

  // using AcceleratorNotationVisitorStrict::visit;

  // // Scalar Expressions
  // void visit(const AcceleratorAccessNode*);
  void visit(const DependencyLiteralNode *);
  void visit(const DependencyVariableNode *);
  // void visit(const AcceleratorSqrtNode*);
  void visit(const AddNode *);
  void visit(const SubNode *);
  void visit(const MulNode *);
  void visit(const DivNode *);
  void visit(const ModNode *);
  void visit(const MetaDataNode *);

  void visit(const IntervalNode *);

  // void visit(const AcceleratorReductionNode*);
  // void visit(const AcceleratorDynamicIndexNode*);

  // void visit(const SubsetNode*);
  void visit(const DataStructureNode *);
  void visit(const ProducerNode *);
  void visit(const ConsumerNode *);
  void visit(const ComputationAnnotationNode *);

  // // Tensor Expressions
  // void visit(const AcceleratorForallNode*);
  // void visit(const AcceleratorAssignmentNode*);

private:
  std::ostream &os;
  // int ident_level;
  void print_ident();

  template <typename Node> void visitBinary(Node op);
  // template <typename Node> void visitImmediate(Node op);
};

} // namespace fern

#endif