#include "codegen/codegen_ir.h"

#include "codegen/codegen_printer.h"
#include "codegen/codegen_visitor.h"

namespace fern {
namespace codegen {

// class Expr
Expr::Expr(bool n) : CGHandle(Literal::make(n)) {}

Expr::Expr(int8_t n) : CGHandle(Literal::make(n)) {}

Expr::Expr(int16_t n) : CGHandle(Literal::make(n)) {}

Expr::Expr(int32_t n) : CGHandle(Literal::make(n)) {}

Expr::Expr(int64_t n) : CGHandle(Literal::make(n)) {}

Expr::Expr(uint8_t n) : CGHandle(Literal::make(n)) {}

Expr::Expr(uint16_t n) : CGHandle(Literal::make(n)) {}

Expr::Expr(uint32_t n) : CGHandle(Literal::make(n)) {}

Expr::Expr(uint64_t n) : CGHandle(Literal::make(n)) {}

Expr::Expr(float n) : CGHandle(Literal::make(n)) {}

Expr::Expr(double n) : CGHandle(Literal::make(n)) {}

Expr::Expr(std::complex<float> n) : CGHandle(Literal::make(n)) {}

Expr::Expr(std::complex<double> n) : CGHandle(Literal::make(n)) {}

Expr Literal::zero(Datatype datatype) {
  Expr zero;
  switch (datatype.getKind()) {
  case Datatype::Bool:
    zero = Literal::make(false);
    break;
  case Datatype::UInt8:
    zero = Literal::make((uint8_t)0);
    break;
  case Datatype::UInt16:
    zero = Literal::make((uint16_t)0);
    break;
  case Datatype::UInt32:
    zero = Literal::make((uint32_t)0);
    break;
  case Datatype::UInt64:
    zero = Literal::make((uint64_t)0);
    break;
  case Datatype::UInt128:
    FERN_ASSERT(false, "unimplemented");
    break;
  case Datatype::Int8:
    zero = Literal::make((int8_t)0);
    break;
  case Datatype::Int16:
    zero = Literal::make((int16_t)0);
    break;
  case Datatype::Int32:
    zero = Literal::make((int32_t)0);
    break;
  case Datatype::Int64:
    zero = Literal::make((int64_t)0);
    break;
  case Datatype::Int128:
    FERN_ASSERT(false, "unimplemented");
    break;
  case Datatype::Float32:
    zero = Literal::make((float)0.0);
    break;
  case Datatype::Float64:
    zero = Literal::make((double)0.0);
    break;
  case Datatype::Complex64:
    zero = Literal::make(std::complex<float>());
    break;
  case Datatype::Complex128:
    zero = Literal::make(std::complex<double>());
    break;
  case Datatype::Undefined:
    FERN_ASSERT(false, "unimplemented");
    break;
  }
  FERN_ASSERT_NO_MSG(zero.defined());
  return zero;
}

Literal::~Literal() { free(val); }

// printing methods
std::ostream &operator<<(std::ostream &os, const Stmt &stmt) {
  if (!stmt.defined())
    return os << "Stmt()" << std::endl;
  CGPrinter printer(os);
  stmt.accept(&printer);
  return os;
}

std::ostream &operator<<(std::ostream &os, const Expr &expr) {
  if (!expr.defined())
    return os << "Expr()";
  CGPrinter printer(os);
  expr.accept(&printer);
  return os;
}

template <> void ExprNode<Literal>::accept(CGVisitorStrict *v) const {
  v->visit((const Literal *)this);
}

template <> void ExprNode<Type>::accept(CGVisitorStrict *v) const {
  v->visit((const Type *)this);
}

template <> void ExprNode<VarDecl>::accept(CGVisitorStrict *v) const {
  v->visit((const VarDecl *)this);
}

template <> void ExprNode<EscapeExpr>::accept(CGVisitorStrict *v) const {
  v->visit((const EscapeExpr *)this);
}

template <> void ExprNode<Var>::accept(CGVisitorStrict *v) const {
  v->visit((const Var *)this);
}

template <> void ExprNode<Call>::accept(CGVisitorStrict *v) const {
  v->visit((const Call *)this);
}

template <> void ExprNode<Gt>::accept(CGVisitorStrict *v) const {
  v->visit((const Gt *)this);
}

template <> void ExprNode<Lt>::accept(CGVisitorStrict *v) const {
  v->visit((const Lt *)this);
}

template <> void ExprNode<Eq>::accept(CGVisitorStrict *v) const {
  v->visit((const Eq *)this);
}

template <> void ExprNode<Neq>::accept(CGVisitorStrict *v) const {
  v->visit((const Neq *)this);
}

template <> void ExprNode<Lte>::accept(CGVisitorStrict *v) const {
  v->visit((const Lte *)this);
}

template <> void ExprNode<Gte>::accept(CGVisitorStrict *v) const {
  v->visit((const Gte *)this);
}

template <> void ExprNode<Add>::accept(CGVisitorStrict *v) const {
  v->visit((const Add *)this);
}

template <> void ExprNode<Sub>::accept(CGVisitorStrict *v) const {
  v->visit((const Sub *)this);
}

template <> void ExprNode<Mul>::accept(CGVisitorStrict *v) const {
  v->visit((const Mul *)this);
}

template <> void ExprNode<Div>::accept(CGVisitorStrict *v) const {
  v->visit((const Div *)this);
}

template <> void ExprNode<Mod>::accept(CGVisitorStrict *v) const {
  v->visit((const Mod *)this);
}

template <> void ExprNode<MetaData>::accept(CGVisitorStrict *v) const {
  v->visit((const MetaData *)this);
}

template <> void StmtNode<VarAssign>::accept(CGVisitorStrict *v) const {
  v->visit((const VarAssign *)this);
}

template <> void StmtNode<For>::accept(CGVisitorStrict *v) const {
  v->visit((const For *)this);
}

template <> void StmtNode<BlankLine>::accept(CGVisitorStrict *v) const {
  v->visit((const BlankLine *)this);
}

template <> void StmtNode<VoidCall>::accept(CGVisitorStrict *v) const {
  v->visit((const VoidCall *)this);
}

template <> void StmtNode<DeclFunc>::accept(CGVisitorStrict *v) const {
  v->visit((const DeclFunc *)this);
}

template <> void StmtNode<EscapeStmt>::accept(CGVisitorStrict *v) const {
  v->visit((const EscapeStmt *)this);
}

template <> void StmtNode<Block>::accept(CGVisitorStrict *v) const {
  v->visit((const Block *)this);
}

template <> void StmtNode<Scope>::accept(CGVisitorStrict *v) const {
  v->visit((const Scope *)this);
}

// template <>
// void StmtNode<CilkFor>::accept(CGVisitorStrict* v)
//     const { v->visit((const CilkFor*)this); }

} // namespace codegen
} // namespace fern