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
  CodeGenerator(Pipeline pipeline) : pipeline(pipeline) {}

  Stmt getCode() const { return code; }
  void generate_code();

private:
  util::ScopedSet<const DependencyVariableNode *> declared;
  Stmt code;
  Pipeline pipeline;
};

std::ostream &operator<<(std::ostream &, const CodeGenerator &);

} // namespace codegen
} // namespace fern

// The actual codegen class goes here.

#endif