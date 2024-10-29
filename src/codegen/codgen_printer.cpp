#include "codegen/codegen_printer.h"

namespace fern {
namespace codegen {

static int codegen_indent = 0;

void CGPrinter::print(Stmt s) { s.accept(this); }

void CGPrinter::visit(const Literal *op) {
  switch (op->type.getKind()) {
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

void CGPrinter::visit(const Scope *op) {
  codegen_indent++;
  os << "{" << std::endl;
  if (op->stmt.defined()) {
    doIdent();
    op->stmt.accept(this);
  }
  os << "}" << std::endl;
  codegen_indent--;
}

void CGPrinter::visit(const Type *op) { os << op->type_name; }

void CGPrinter::doIdent() {
  for (int i = 0; i < codegen_indent; i++) {
    os << " ";
  }
}

void CGPrinter::visit(const VarDecl *op) {
  if (op->is_const)
    os << "const ";
  os << op->type;

  for (int i = 0; i < op->num_ref; i++) {
    os << "&";
  }

  for (int i = 0; i < op->num_ptr; i++) {
    os << "*";
  }

  os << " " << op->var_name;
}

void CGPrinter::visit(const DeclFunc *op) {
  doIdent();
  os << op->return_type << " " << op->name << "(";
  if (op->args.size() != 0) {
    for (int i = 0; i < op->args.size() - 1; i++) {
      os << op->args[i] << ", ";
    }
    os << op->args[op->args.size() - 1];
  }
  os << ")";
  os << op->body << std::endl;
}

void CGPrinter::visit(const Block *op) {
  for (auto stmt : op->stmts) {
    doIdent();
    os << stmt << std::endl;
  }
}

void CGPrinter::visit(const EscapeExpr *op) { os << op->code; }

void CGPrinter::visit(const EscapeStmt *op) { os << op->code; }

void CGPrinter::visit(const VarAssign *op) {
  os << op->lhs << " = " << op->rhs << ";";
}

void CGPrinter::visit(const Var *op) { os << op->name; }

void CGPrinter::visit(const Call *op) {
  os << op->name;

  if (op->arg.size() == 0) {
    os << "()";
  } else {
    os << "(";
    for (int i = 0; i < op->arg.size() - 1; i++) {
      os << op->arg[i] << ", ";
    }
    os << op->arg[op->arg.size() - 1] << ")";
  }
}

void CGPrinter::visit(const VoidCall *op) {
  doIdent();
  os << op->func << ";";
}

void CGPrinter::visit(const BlankLine *op) { os << std::endl; }

void CGPrinter::visit(const For *op) {
  auto step_ptr = to<VarAssign>(op->step);
  if (op->parallel) {
    os << "cilk_scope {" << std::endl;
    codegen_indent++;
    doIdent();
    os << "cilk_";
  }
  os << "for(" << op->start << op->cond << "; " << step_ptr->lhs;

  switch (step_ptr->op) {
  case AddEQ:
    os << "+=";
    break;
  case SubEQ:
    os << "-=";
    break;
  case MulEQ:
    os << "*=";
    break;
  case EQ:
    os << "=";
    break;
  default:
    FERN_ASSERT(false, "unreachable");
    break;
  }

  os << step_ptr->rhs << ")";
  os << op->body;

  if (op->parallel) {
    doIdent();
    os << "}" << std::endl;
    codegen_indent--;
  }
}

void CGPrinter::visit(const Lt *op) { os << op->a << " < " << op->b; }
void CGPrinter::visit(const Gt *op) { os << op->a << " > " << op->b; }
void CGPrinter::visit(const Eq *op) { os << op->a << " == " << op->b; }
void CGPrinter::visit(const Neq *op) { os << op->a << " != " << op->b; }
void CGPrinter::visit(const Gte *op) { os << op->a << " >= " << op->b; }
void CGPrinter::visit(const Lte *op) { os << op->a << " <= " << op->b; }
void CGPrinter::visit(const Add *op) { os << op->a << " + " << op->b; }
void CGPrinter::visit(const Mul *op) { os << op->a << " * " << op->b; }
void CGPrinter::visit(const Div *op) { os << op->a << " / " << op->b; }
void CGPrinter::visit(const Mod *op) { os << op->a << " % " << op->b; }
void CGPrinter::visit(const Sub *op) { os << op->a << " - " << op->b; }

void CGPrinter::visit(const MetaData *op) { os << op->var << "." << op->field; }

// Python Printing
void PyCGPrinter::print(Stmt s) { s.accept(this); }

void PyCGPrinter::visit(const Literal *op) {
  switch (op->type.getKind()) {
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

void PyCGPrinter::visit(const Scope *op) {
  if (op->stmt.defined()) {
    doIdent();
    op->stmt.accept(this);
  }
}

void PyCGPrinter::visit(const Type *op) {}

void PyCGPrinter::doIdent() {
  for (int i = 0; i < ident; i++) {
    os << "\t";
  }
}

void PyCGPrinter::visit(const VarDecl *op) { os << op->var_name; }

void PyCGPrinter::visit(const DeclFunc *op) {
  doIdent();
  os << "def " << op->name << "(";
  if (op->args.size() != 0) {
    for (int i = 0; i < op->args.size() - 1; i++) {
      op->args[i].accept(this);
      os << ", ";
    }
    op->args[op->args.size() - 1].accept(this);
  }
  os << "):" << std::endl;
  ident++;
  op->body.accept(this);
  os << std::endl;
  ident--;
}

void PyCGPrinter::visit(const Block *op) {
  os << std::endl;
  for (auto stmt : op->stmts) {
    doIdent();
    stmt.accept(this);
    os << std::endl;
  }
}

void PyCGPrinter::visit(const EscapeExpr *op) { os << op->code; }

void PyCGPrinter::visit(const EscapeStmt *op) { os << op->code; }

void PyCGPrinter::visit(const VarAssign *op) {
  op->lhs.accept(this);
  os << " = ";
  op->rhs.accept(this);
}

void PyCGPrinter::visit(const Var *op) { os << op->name; }

void PyCGPrinter::visit(const Call *op) {
  os << op->name;

  if (op->arg.size() == 0) {
    os << "()";
  } else {
    os << "(";
    for (int i = 0; i < op->arg.size() - 1; i++) {
      os << op->arg[i] << ", ";
    }
    os << op->arg[op->arg.size() - 1] << ")";
  }
}

void PyCGPrinter::visit(const VoidCall *op) { op->func.accept(this); }

void PyCGPrinter::visit(const BlankLine *op) { os << std::endl; }

void PyCGPrinter::visit(const For *op) {
  auto step_ptr = to<VarAssign>(op->step);
  auto start_ptr = to<VarAssign>(op->start);
  auto end_ptr = to<Lt>(op->cond);

  if (op->parallel) {
    FERN_ASSERT(false, "Parrallel for for Python is unimplemented");
  }
  os << "for ";
  step_ptr->lhs.accept(this);
  os << " in range(";
  start_ptr->rhs.accept(this);
  os << ", ";
  end_ptr->b.accept(this);
  os << ", ";
  step_ptr->rhs.accept(this);
  os << "):" << std::endl;

  ident++;
  op->body.accept(this);
  ident--;
}

void PyCGPrinter::visit(const Lt *op) {
  op->a.accept(this);
  os << " < ";
  op->b.accept(this);
}

void PyCGPrinter::visit(const Gt *op) {
  op->a.accept(this);
  os << " > ";
  op->b.accept(this);
}

void PyCGPrinter::visit(const Eq *op) {
  op->a.accept(this);
  os << " == ";
  op->b.accept(this);
}

void PyCGPrinter::visit(const Neq *op) {
  op->a.accept(this);
  os << " != ";
  op->b.accept(this);
}

void PyCGPrinter::visit(const Gte *op) {
  op->a.accept(this);
  os << " >= ";
  op->b.accept(this);
}

void PyCGPrinter::visit(const Lte *op) {
  op->a.accept(this);
  os << " <= ";
  op->b.accept(this);
}
void PyCGPrinter::visit(const Add *op) {
  op->a.accept(this);
  os << " + ";
  op->b.accept(this);
}
void PyCGPrinter::visit(const Mul *op) {
  op->a.accept(this);
  os << " * ";
  op->b.accept(this);
}
void PyCGPrinter::visit(const Div *op) {
  op->a.accept(this);
  os << " / ";
  op->b.accept(this);
}
void PyCGPrinter::visit(const Mod *op) {
  op->a.accept(this);
  os << " % ";
  op->b.accept(this);
}
void PyCGPrinter::visit(const Sub *op) {
  op->a.accept(this);
  os << " - ";
  op->b.accept(this);
}

void PyCGPrinter::visit(const MetaData *op) {
  op->var.accept(this);
  os << "." << op->field;
}

} // namespace codegen
} // namespace fern