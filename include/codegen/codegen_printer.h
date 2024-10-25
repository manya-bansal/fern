#ifndef CODEGEN_PRINTER_H
#define CODEGEN_PRINTER_H

#include "codegen/codegen_ir.h"
#include "codegen/codegen_visitor.h"

namespace fern {
namespace codegen {

class CGPrinter : public CGVisitorStrict {
public:
  CGPrinter(std::ostream &os) : os(os) {}
  virtual ~CGPrinter() {}

  void print(Stmt);

  virtual void visit(const Literal *);
  virtual void visit(const Scope *);
  virtual void visit(const Type *);
  virtual void visit(const VarDecl *);
  virtual void visit(const DeclFunc *);
  virtual void visit(const Block *);
  virtual void visit(const EscapeExpr *);
  virtual void visit(const EscapeStmt *);
  virtual void visit(const VarAssign *);
  virtual void visit(const Var *);
  virtual void visit(const Call *);
  virtual void visit(const VoidCall *);
  virtual void visit(const BlankLine *);
  virtual void visit(const For *);
  virtual void visit(const Lt *);
  virtual void visit(const Gt *);
  virtual void visit(const Eq *);
  virtual void visit(const Neq *);
  virtual void visit(const Gte *);
  virtual void visit(const Lte *);
  virtual void visit(const Add *);
  virtual void visit(const Mul *);
  virtual void visit(const Div *);
  virtual void visit(const Mod *);
  virtual void visit(const Sub *);
  virtual void visit(const MetaData *);
  // virtual void visit(const CilkFor*);

private:
  std::ostream &os;
  void doIdent();
};


class PyCGPrinter : public CGVisitorStrict {
public:
  PyCGPrinter(std::ostream &os) : os(os) {}
  virtual ~PyCGPrinter() {}

  void print(Stmt);

  virtual void visit(const Literal *);
  virtual void visit(const Scope *);
  virtual void visit(const Type *);
  virtual void visit(const VarDecl *);
  virtual void visit(const DeclFunc *);
  virtual void visit(const Block *);
  virtual void visit(const EscapeExpr *);
  virtual void visit(const EscapeStmt *);
  virtual void visit(const VarAssign *);
  virtual void visit(const Var *);
  virtual void visit(const Call *);
  virtual void visit(const VoidCall *);
  virtual void visit(const BlankLine *);
  virtual void visit(const For *);
  virtual void visit(const Lt *);
  virtual void visit(const Gt *);
  virtual void visit(const Eq *);
  virtual void visit(const Neq *);
  virtual void visit(const Gte *);
  virtual void visit(const Lte *);
  virtual void visit(const Add *);
  virtual void visit(const Mul *);
  virtual void visit(const Div *);
  virtual void visit(const Mod *);
  virtual void visit(const Sub *);
  virtual void visit(const MetaData *);
  // virtual void visit(const CilkFor*);

private:
  std::ostream &os;
  int ident = 0;
  void doIdent();
};


} // namespace codegen
} // namespace fern

#endif