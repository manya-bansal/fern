
#include "codegen/codegen.h"

namespace fern {
namespace codegen {
void CodeGenerator::generate_code() {}

std::ostream &operator<<(std::ostream &os, const CodeGenerator &cg) {
  os << cg.getCode();
  return os;
}
} // namespace codegen
} // namespace fern