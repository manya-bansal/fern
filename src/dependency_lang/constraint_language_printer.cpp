#include "dependency_lang/dep_lang_printer.h"

#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"

namespace fern {

static int ident_level = 0;

DependencyPrinter::DependencyPrinter(std::ostream &os) : os(os) {
  // ident_level = 0;
}

// DependencyPrinter::DependencyPrinter(std::ostream& os, int ident_level) :
// os(os), ident_level(ident_level) {
// }

void DependencyPrinter::print(const DependencyExpr &expr) { expr.accept(this); }

void DependencyPrinter::print(const DependencySubset &expr) {
  expr.accept(this);
}

void DependencyPrinter::print(const DependencyIter &expr) { expr.accept(this); }

void DependencyPrinter::visit(const DependencyLiteralNode *op) {
  switch (op->getDataType().getKind()) {
  case Datatype::Bool:
    os << op->getVal<bool>();
    break;
  case Datatype::UInt8:
    os << op->getVal<uint8_t>();
    break;
  case Datatype::UInt16:
    os << op->getVal<uint16_t>();
    break;
  case Datatype::UInt32:
    os << op->getVal<uint32_t>();
    break;
  case Datatype::UInt64:
    os << op->getVal<uint64_t>();
    break;
  case Datatype::UInt128:
    std::cerr << "Not supported" << std::endl;
    break;
  case Datatype::Int8:
    os << op->getVal<int8_t>();
    break;
  case Datatype::Int16:
    os << op->getVal<int16_t>();
    break;
  case Datatype::Int32:
    os << op->getVal<int32_t>();
    break;
  case Datatype::Int64:
    os << op->getVal<int64_t>();
    break;
  case Datatype::Int128:
    std::cerr << "Not supported" << std::endl;
    break;
  case Datatype::Float32:
    os << op->getVal<float>();
    break;
  case Datatype::Float64:
    os << op->getVal<double>();
    break;
  case Datatype::Complex64:
    os << op->getVal<std::complex<float>>();
    break;
  case Datatype::Complex128:
    os << op->getVal<std::complex<double>>();
    break;
  case Datatype::Undefined:
    break;
  }
}

void DependencyPrinter::visit(const DependencyVariableNode *op) {
  os << op->name;
  if (op->argument) {
    os << " (Argument) ";
  }
  if (op->parallelize) {
    os << " (Parrallel)";
  }
}

template <typename Node> void DependencyPrinter::visitBinary(Node op) {
  op->a.accept(this);
  os << " " << op->getOperatorString() << " ";
  op->b.accept(this);
}

void DependencyPrinter::visit(const AddNode *op) { visitBinary(op); }

void DependencyPrinter::visit(const SubNode *op) { visitBinary(op); }

void DependencyPrinter::visit(const MulNode *op) { visitBinary(op); }

void DependencyPrinter::visit(const DivNode *op) { visitBinary(op); }

void DependencyPrinter::visit(const ModNode *op) { visitBinary(op); }

void DependencyPrinter::visit(const MetaDataNode *node) {
  os << node->ds->getVarName() << "." << node->name;
}

void DependencyPrinter::visit(const IntervalNode *op) {
  op->var.accept(this);
  os << " in [";
  op->start.accept(this);
  os << " ,";
  op->end.accept(this);
  os << " ,";
  op->step.accept(this);
  os << "]";

  if (op->recompute) {
    os << "  @ Recompute";
  }
  os << " }" << std::endl;

  if (op->child != nullptr) {
    ident_level++;
    print_ident();
    op->child.accept(this);
    ident_level--;
    print_ident();
    os << "}" << std::endl;
  } else {
    std::cout << "interval has no child" << std::endl;
  }
}

// void DependencyPrinter::visit(const SubsetNode* op) {
//     os << op->meta;
//     switch (op->op) {
//         case EQ:
//             os << "=";
//             break;
//         case GEQ:
//             os << ">=";
//             break;
//         case LEQ:
//             os << "<=";
//             break;
//         case GREATER:
//             os << ">";
//             break;
//         case LESS:
//             os << ">";
//             break;
//         default:
//             break;
//     }
//     os << op->expr;
// }

void DependencyPrinter::visit(const DataStructureNode *op) {
  os << op->abstractData->getTypeName() << " " << op->name << "{" << std::endl;
  ident_level++;
  // std::cout << op->annotations.size() << std::endl;
  // std::cout << op->abstractData.getMetaData().size() << std::endl;
  assert(op->annotations.size() == op->abstractData->getMetaData().size());
  auto metaDataFields = op->abstractData->getMetaData();

  for (int i = 0; i < op->annotations.size(); i++) {
    print_ident();
    os << metaDataFields[i] << ": " << op->annotations[i];
    os << "," << std::endl;
  }
  ident_level--;
  print_ident();
  os << "}" << std::endl;
}

void DependencyPrinter::visit(const ProducerNode *op) {
  os << "Produce(" << std::endl;
  ident_level++;
  print_ident();
  os << op->child;
  ident_level--;
  print_ident();
  os << ")" << std::endl;
}

void DependencyPrinter::visit(const ConsumerNode *op) {
  os << "Consume(" << std::endl;
  ident_level++;
  for (auto &consumer : op->child) {
    print_ident();
    os << consumer;
  }
  ident_level--;
  print_ident();
  os << ")" << std::endl;
}

void DependencyPrinter::visit(const ComputationAnnotationNode *op) {
  os << "Computation (" << std::endl;
  ident_level++;
  print_ident();
  os << op->producer;
  print_ident();
  os << op->consumer;
  ident_level--;
  print_ident();
  os << ")" << std::endl;
}

void DependencyPrinter::print_ident() {
  for (int i = 0; i < ident_level; i++) {
    os << ' ';
  }
}

} // namespace fern