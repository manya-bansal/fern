#ifndef CODEGEN_H
#define CODEGEN_H

#include "codegen/codegen_ir.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "dependency_lang/dep_lang_rewriter.h"
#include "dependency_resolver/pipeline.h"
#include "utils/scopedset.h"

namespace fern {
namespace codegen {
class CodeGenerator {
public:
  CodeGenerator(Pipeline pipeline) : pipeline(pipeline) {
    code = generate_code();
  }

  Stmt getCode() const { return code; }
  Stmt generate_code();
  Expr
  generate_expr(DependencyExpr e,
                const std::set<const DependencyVariableNode *> &declared_var,
                bool check_decl = true) const;

private:
  util::ScopedSet<const DependencyVariableNode *> declared;
  Stmt code;
  Pipeline pipeline;

  // Functions to generate stmts for each pipeline node
  Stmt generateQueryNode(const QueryNode *node);
  Stmt generateComputeNode(const ComputeNode *node);
  Stmt generatAllocateNode(const AllocateNode *node);
  Stmt generateInsertNode(const InsertNode *node);
  Stmt generateFreeNode(const FreeNode *node);
  Stmt generatePipelineNode(const PipelineNode *node);
};

std::ostream &operator<<(std::ostream &, const CodeGenerator &);

} // namespace codegen
} // namespace fern

// The actual codegen class goes here.

#endif