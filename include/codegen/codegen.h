#ifndef CODEGEN_H
#define CODEGEN_H

#include "codegen/codegen_ir.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "dependency_lang/dep_lang_rewriter.h"
#include "dependency_resolver/pipeline.h"
#include "utils/scopedset.h"

#include <set>

namespace fern {
namespace codegen {
class CodeGenerator {
public:
  CodeGenerator(Pipeline pipeline);
  Stmt getCode() const { return code; }
  Stmt generateCode();
  Stmt getFullFunction() const;
  std::vector<Expr> generateFunctionHeaderArguments();
  Expr
  generateExpr(DependencyExpr e,
               const std::set<const DependencyVariableNode *> &declared_var,
               bool check_decl = true) const;
  std::set<const DependencyVariableNode *> arg_vars;

private:
  std::set<const DependencyVariableNode *> declared_var;
  Stmt fullFunction;
  Stmt code;
  Pipeline pipeline;

  void populateIntervalVars();

  // Functions to generate stmts for each pipeline node
  Stmt generateQueryNode(const QueryNode *node) const;
  Stmt generateComputeNode(const ComputeNode *node) const;
  Stmt generatAllocateNode(const AllocateNode *node) const;
  Stmt generateInsertNode(const InsertNode *node) const;
  Stmt generateFreeNode(const FreeNode *node) const;
  Stmt generatePipelineNode(const PipelineNode *node);
};

std::ostream &operator<<(std::ostream &, const CodeGenerator &);

} // namespace codegen
} // namespace fern

// The actual codegen class goes here.

#endif