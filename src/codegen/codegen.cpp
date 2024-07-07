
#include "codegen/codegen.h"

namespace fern {
namespace codegen {

Expr CodeGenerator::generate_expr(
    DependencyExpr e,
    const std::set<const DependencyVariableNode *> &declared_var) const {
  Expr new_e;
  match(e,
        std::function<void(const DependencyVariableNode *, Matcher *)>(
            [&](const DependencyVariableNode *op, Matcher *ctx) {
              if (pipeline.bounded_vars.count(Variable(op)) > 0) {
                new_e = Literal::make(pipeline.bounded_vars.at(Variable(op)),
                                      Int64);
              } else if (declared_var.count(op)) {
                new_e = Var::make(op->name);
              } else {
                new_e = VarDecl::make(codegen::Type::make(op->type.getString()),
                                      op->name);
              }
              //  new_e = codegen::Var::make(op->getVal<int64_t>(), Int64);
            }),
        std::function<void(const DependencyLiteralNode *, Matcher *)>(
            [&](const DependencyLiteralNode *op, Matcher *ctx) {
              new_e = Literal::make(op->getVal<int64_t>(), Int64);
            }),
        std::function<void(const AddNode *, Matcher *)>(
            [&](const AddNode *op, Matcher *ctx) {
              ctx->match(op->a);
              auto save_a = new_e;
              ctx->match(op->b);
              auto save_b = new_e;
              new_e = Add::make(save_a, save_b);
            }),
        std::function<void(const SubNode *, Matcher *)>(
            [&](const SubNode *op, Matcher *ctx) {
              ctx->match(op->a);
              auto save_a = new_e;
              ctx->match(op->b);
              auto save_b = new_e;
              new_e = Sub::make(save_a, save_b);
            }),
        std::function<void(const MulNode *, Matcher *)>(
            [&](const MulNode *op, Matcher *ctx) {
              ctx->match(op->a);
              auto save_a = new_e;
              ctx->match(op->b);
              auto save_b = new_e;
              new_e = Mul::make(save_a, save_b);
            }),
        std::function<void(const DivNode *, Matcher *)>(
            [&](const DivNode *op, Matcher *ctx) {
              ctx->match(op->a);
              auto save_a = new_e;
              ctx->match(op->b);
              auto save_b = new_e;
              new_e = Div::make(save_a, save_b);
            }));
  return new_e;
}

Stmt CodeGenerator::generate_code() {

  std::vector<Stmt> stmts;
  std::set<const DependencyVariableNode *> declared_var;

  // Set all the loop variables to be declared
  // We will generate these later, but do not want to
  // redeclare
  for (auto l : pipeline.outer_loops) {
    declared_var.insert(getNode(l.var));
  }

  // Declare all the derived vars
  for (auto v : pipeline.derivations) {
    if (declared_var.count(getNode(get<0>(v))) > 0)
      continue;
    auto lhs = generate_expr(get<0>(v), declared_var);
    declared_var.insert(getNode(get<0>(v)));
    auto rhs = generate_expr(get<1>(v), declared_var);
    stmts.push_back(VarAssign::make(lhs, rhs));
  }

  // Declare all the var relationships
  for (auto v : pipeline.var_relationships_sols) {
    if (declared_var.count(v.var) > 0)
      continue;
    auto var_decl = generate_expr(Variable(v.var), declared_var);
    declared_var.insert(v.var);

    std::stringstream ss;
    ss << v.solution;
    stmts.push_back(VarAssign::make(var_decl, EscapeExpr::make(ss.str())));
  }

  return Block::make(stmts);
}

std::ostream &operator<<(std::ostream &os, const CodeGenerator &cg) {
  os << cg.getCode();
  return os;
}

} // namespace codegen
} // namespace fern