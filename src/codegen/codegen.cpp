
#include "codegen/codegen.h"

namespace fern {
namespace codegen {

Expr CodeGenerator::generate_expr(
    DependencyExpr e,
    const std::set<const DependencyVariableNode *> &declared_var,
    bool check_decl) const {
  Expr new_e;
  match(e,
        std::function<void(const DependencyVariableNode *, Matcher *)>(
            [&](const DependencyVariableNode *op, Matcher *ctx) {
              if (pipeline.bounded_vars.count(Variable(op)) > 0) {
                new_e = Literal::make(pipeline.bounded_vars.at(Variable(op)),
                                      Int64);
                return;
              }

              new_e = Var::make(op->name);

              if (check_decl) {
                if (declared_var.count(op) <= 0) {
                  new_e = VarDecl::make(
                      codegen::Type::make(op->type.getString()), op->name);
                }
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

  // Now start generating the function calls
  for (auto func : pipeline.pipeline) {
    switch (func.getFuncType()) {
    case BLANK:
      // skip entirely
      break;
    case ALLOCATE:
      stmts.push_back(generatAllocateNode(func.getNode<AllocateNode>()));
      break;
    case COMPUTE:
      stmts.push_back(generateComputeNode(func.getNode<ComputeNode>()));
      break;
    case QUERY:
      stmts.push_back(generateQueryNode(func.getNode<QueryNode>()));
      break;
    case FREE:
      stmts.push_back(generateFreeNode(func.getNode<FreeNode>()));
      break;
    case INSERT:
      stmts.push_back(generateInsertNode(func.getNode<InsertNode>()));
      break;
    case PIPELINE:
      stmts.push_back(generatePipelineNode(func.getNode<PipelineNode>()));
      break;
    default:
      // Unreachable
      FERN_ASSERT_NO_MSG(false);
      break;
    }
  }

  // Finally generate the outerloops
  // auto lowered_code = Block::make(stmts);
  // auto inner_loop_index = pipeline.outer_loops.size() - 1;
  // for (int i = inner_loop_index; i >= 0; i--) {
  //   lowered_code = For::make()
  // }

  return Block::make(stmts);
}

Stmt CodeGenerator::generateQueryNode(const QueryNode *node) {
  std::vector<Expr> meta_data_vals;
  for (auto e : node->deps) {
    // Never want to put a declaration function arguments.
    meta_data_vals.push_back(generate_expr(e, {}, false));
  }

  auto var_decl =
      VarDecl::make(Type::make(node->ds->getTypeName()), node->child);
  auto func_call = Call::make(
      node->parent + "." + node->ds->getDataQueryInterface(), meta_data_vals);

  return VarAssign::make(var_decl, func_call);
}

static Expr
lowerArguments(Argument arg,
               std::map<const AbstractDataStructure *, std::string> names) {

  switch (arg.getArgType()) {
  case INT: {
    auto num = arg.getNode<LiteralArg>();
    std::stringstream ss;
    num->print(ss);
    return EscapeExpr::make(ss.str());
  }

  case STRING: {
    auto s = arg.getNode<StringArg>();
    return EscapeExpr::make(s->getString());
    break;
  }

  case VARIABLE: {
    auto var = arg.getNode<VariableArg>()->getVariable();
    return Var::make(var->name);
  }

  case DUMMY_DATASTRUCTURE: {
    auto ds = arg.getNode<DummyDataStructureArg>();
    return Var::make(ds->dsPtr()->getVarName());
  }

  case DATASTRUCTURE: {
    auto ds = arg.getNode<DataStructureArg>()->dsPtr();
    FERN_ASSERT(names.count(ds) > 0, "Should already have a name ready");
    return Var::make(names[ds]);
  }

  default:
    FERN_ASSERT(false, "Unreachable");
  }
}

Stmt CodeGenerator::generateComputeNode(const ComputeNode *node) {

  std::cout << node->func << std::endl;

  std::vector<Expr> args;
  for (auto arg : node->func.getArguments()) {
    args.push_back(lowerArguments(arg, node->names));
  }
  return VoidCall::make(Call::make(node->func.getName(), args));
}

Stmt CodeGenerator::generatAllocateNode(const AllocateNode *node) {
  std::vector<Expr> meta_data_vals;
  for (auto e : node->deps) {
    // Never want to put a declaration function arguments.
    meta_data_vals.push_back(generate_expr(e, {}, false));
  }

  auto var_decl =
      VarDecl::make(Type::make(node->ds->getTypeName()), node->name);
  auto func_call = Call::make(node->ds->getAllocData(), meta_data_vals);

  return VarAssign::make(var_decl, func_call);
}

Stmt CodeGenerator::generateInsertNode(const InsertNode *node) {
  std::vector<Expr> meta_data_vals;
  for (auto e : node->deps) {
    // Never want to put a declaration function arguments.
    meta_data_vals.push_back(generate_expr(e, {}, false));
  }

  meta_data_vals.push_back(EscapeExpr::make(node->child));

  auto insert_call = Call::make(
      node->parent + "." + node->ds->getDataInsertInterface(), meta_data_vals);

  return VoidCall::make(insert_call);
}

Stmt CodeGenerator::generateFreeNode(const FreeNode *node) {

  std::string func_name;
  if (node->allocate) {
    func_name = node->ds->getAllocFreeInterface();
  } else {
    func_name = node->ds->getQueryFreeInterface();
  }
  return VoidCall::make(Call::make(node->name + "." + func_name, {}));
}

Stmt CodeGenerator::generatePipelineNode(const PipelineNode *node) {
  CodeGenerator cg(node->pipeline);
  return cg.getCode();
}

std::ostream &operator<<(std::ostream &os, const CodeGenerator &cg) {
  os << cg.getCode();
  return os;
}

} // namespace codegen
} // namespace fern