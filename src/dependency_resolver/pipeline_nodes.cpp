#include "dependency_resolver/pipeline_nodes.h"

namespace fern {

std::ostream &operator<<(std::ostream &os, const FunctionType &f) {
  f.getNode()->print(os);
  return os;
}

void FunctionTypeNode::print(std::ostream &stream) const {
  stream << "Print Method not defined on Function type" << std::endl;
}

Action FunctionType::getFuncType() const { return getNode()->action; }

void QueryNode::print(std::ostream &stream) const {
  stream << child << " = " << parent << ".query(";
  for (const auto &d : deps) {
    stream << d << " , ";
  }
  stream << ")";
}

void AllocateNode::print(std::ostream &stream) const {
  stream << name << " = allocate(";
  for (const auto &d : deps) {
    stream << d << " , ";
  }
  stream << ")";
}

void AllocateNode::set_reuse() { reuse = true; }

void InsertNode::print(std::ostream &stream) const {
  stream << parent << ".insert(";
  for (const auto &d : deps) {
    stream << d << " , ";
  }
  stream << child << ")";
  // stream << child << " = " << parent << " .insert()";
}

void FreeNode::print(std::ostream &stream) const { stream << name << "free()"; }

void ComputeNode::print(std::ostream &stream) const {
  stream << func.getName() << "(";
  for (auto m : names) {
    stream << m.first->getVarName() << " : " << m.second << ", ";
  }
  stream << ")";
}

void BlankNode::print(std::ostream &stream) const { stream << " BLANK "; }

void PipelineNode::print(std::ostream &stream) const { stream << pipeline; }

} // namespace fern