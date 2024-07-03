#include "dependency_lang/dep_lang_nodes_abstract.h"

using namespace std;

namespace fern {

DependencyExprNode::DependencyExprNode(Datatype type) : dataType(type) {}

Datatype DependencyExprNode::getDataType() const { return dataType; }

// AcceleratorStmtNode::AcceleratorStmtNode(Type type) : type (type) {}

// Type AcceleratorStmtNode::getType() const {
//     return type;
// }

} // namespace fern