#include "dependency_lang/dep_lang_visitor.h"

#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"

namespace fern {
void DependencyExprVisitorStrict::visit(const DependencyExpr &expr) {
  std::cout << "here expr" << std::endl;
  if (expr.defined()) {
    expr.accept(this);
  }
}

void DependencySubsetVisitorStrict::visit(const DependencySubset &expr) {
  std::cout << "here subset" << std::endl;
  if (expr.defined()) {
    expr.accept(this);
  }
}

void DependencyVisitor::visit(const DependencyLiteralNode *node) {}

void DependencyVisitor::visit(const DependencyVariableNode *node) {}

// void DependencyVisitor::visit(const DependencyArgumentNode* node) {
// }

void DependencyVisitor::visit(const AddNode *node) {
  node->a.accept(this);
  node->b.accept(this);
}

void DependencyVisitor::visit(const SubNode *node) {
  node->a.accept(this);
  node->b.accept(this);
}

void DependencyVisitor::visit(const DivNode *node) {
  node->a.accept(this);
  node->b.accept(this);
}

void DependencyVisitor::visit(const MulNode *node) {
  node->a.accept(this);
  node->b.accept(this);
}

void DependencyVisitor::visit(const ModNode *node) {
  node->a.accept(this);
  node->b.accept(this);
}

void DependencyVisitor::visit(const MetaDataNode *node) {}

// void DependencyVisitor::visit(const SubsetNode* node) {
//     node->expr.accept(this);
// }

void DependencyVisitor::visit(const DataStructureNode *node) {
  for (auto &annot : node->annotations) {
    annot.accept(this);
  }
}

void DependencyVisitor::visit(const ProducerNode *node) {
  node->child.accept(this);
}

void DependencyVisitor::visit(const ConsumerNode *node) {
  for (auto &consumer : node->child) {
    consumer.accept(this);
  }
}

void DependencyVisitor::visit(const ComputationAnnotationNode *node) {
  node->producer.accept(this);
  node->consumer.accept(this);
}

void DependencyVisitor::visit(const IntervalNode *node) {
  node->var.accept(this);
  node->start.accept(this);
  node->end.accept(this);
  node->step.accept(this);
  node->child.accept(this);
}

} // namespace fern