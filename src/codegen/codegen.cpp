
#include "codegen/codegen.h"
#include "utils/printer.h"

namespace fern {
namespace codegen {

CodeGenerator::CodeGenerator(Pipeline pipeline) : pipeline(pipeline) {
  code = generateCode();
  auto args = generateFunctionHeaderArguments();
  fullFunction =
      DeclFunc::make("my_fused_impl", codegen::Type::make("void"), args, code);
}

Expr CodeGenerator::generateExpr(
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
                  new_e =
                      VarDecl::make(Type::make(op->type.getString()), op->name);
                }
              }
            }),
        std::function<void(const DependencyLiteralNode *, Matcher *)>(
            [&](const DependencyLiteralNode *op, Matcher *ctx) {
              new_e = Literal::make(op->getVal<int64_t>(), Int64);
            }),
        std::function<void(const MetaDataNode *, Matcher *)>(
            [&](const MetaDataNode *op, Matcher *ctx) {
              new_e = MetaData::make(Var::make(op->ds->getVarName()), op->name);
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

Stmt CodeGenerator::generateCode() {
  std::vector<Stmt> stmts;

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
    auto lhs = generateExpr(get<0>(v), declared_var);
    declared_var.insert(getNode(get<0>(v)));
    auto rhs = generateExpr(get<1>(v), declared_var, false);
    stmts.push_back(VarAssign::make(lhs, rhs));
  }

  // Declare all the var relationships
  for (auto v : pipeline.var_relationships_sols) {
    if (declared_var.count(v.var) > 0)
      continue;
    auto var_decl = generateExpr(Variable(v.var), declared_var);
    declared_var.insert(v.var);

    std::stringstream ss;
    ss << v.solution;
    stmts.push_back(VarAssign::make(var_decl, EscapeExpr::make(ss.str())));
  }

  for (auto rel_nodes : pipeline.merge_rel_nodes) {
    stmts.push_back(generateRelNodes(rel_nodes));
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
  auto lowered_code = Block::make(stmts);
  auto inner_loop_index = pipeline.outer_loops.size() - 1;

  std::cout << Block::make(stmts) << std::endl;

  // Move from inner to outer
  for (int i = inner_loop_index; i >= 0; i--) {
    auto loop = pipeline.outer_loops[i];
    // Delete outer loop vars from declared so that we can decalre in for loops
    declared_var.erase(getNode(loop.var));
    Stmt start = VarAssign::make(generateExpr(loop.var, declared_var),
                                 generateExpr(loop.start, declared_var, false));
    declared_var.insert(getNode(loop.var));

    Expr end = Lt::make(generateExpr(loop.var, declared_var),
                        generateExpr(loop.end, declared_var, false));

    Stmt step =
        VarAssign::make(generateExpr(loop.var, declared_var, false),
                        generateExpr(loop.step, declared_var, false), AddEQ);

    // Do we need to push out a declaration of step?
    // get all the vars in step
    auto all_vars = loop.step.getSymbols();
    for (const auto &v : all_vars) {
      if (declared_var.count(v) > 0) {
        continue;
      }
      if (pipeline.bounded_vars.count(Variable(v)) > 0) {
        continue;
      }
      // std::cout << "Inserting " << Variable(v) << std::endl;
      arg_vars.insert(v);
    }

    lowered_code =
        For::make(start, end, step, lowered_code, loop.var.isParallel());
  }

  std::cout << Block::make(lowered_code) << std::endl;

  return lowered_code;
}

Stmt CodeGenerator::generateQueryNode(const QueryNode *node) const {
  std::vector<Expr> meta_data_vals;
  for (auto e : node->deps) {
    // Never want to put a declaration function arguments.
    meta_data_vals.push_back(generateExpr(e, {}, false));
  }

  auto var_decl =
      VarDecl::make(Type::make(node->ds->getTypeName()), node->child);
  auto func_call = Call::make(
      node->parent + "." + node->ds->getDataQueryInterface(), meta_data_vals);

  return VarAssign::make(var_decl, func_call);
}

Stmt CodeGenerator::generateRelNodes(NodeMergeRel rel) {
  std::vector<Stmt> decls;
  auto v_rel = rel.v_rel;
  auto step_rel = rel.step_rel;
  FERN_ASSERT_NO_MSG(v_rel.size() == step_rel.size());
  FERN_ASSERT_NO_MSG(v_rel.size() > 0);

  Variable merge(util::uniqueName("merge"));

  auto v_decl = generateExpr(rel.v, {}, true);
  auto step_decl = generateExpr(merge, {}, true);

  decls.push_back(VarAssign::make(v_decl, generateExpr(v_rel[0], {}, false)));
  decls.push_back(VarAssign::make(
      step_decl, generateExpr(v_rel[0] + step_rel[0], {}, false)));

  auto v = generateExpr(rel.v, {}, false);
  auto m = generateExpr(merge, {}, false);
  for (auto i = 1; i < v_rel.size(); i++) {
    auto v_next = generateExpr(v_rel[i], {}, false);
    auto m_next = generateExpr(v_rel[i] + step_rel[i], {}, false);

    Expr min_call = Call::make("std::min", {v, v_next});
    Expr max_call = Call::make("std::max", {m, m_next});

    decls.push_back(VarAssign::make(v, min_call));
    decls.push_back(VarAssign::make(m, max_call));
  }

  decls.push_back(VarAssign::make(generateExpr(rel.step, {}, true),
                                  generateExpr(merge - rel.v, {}, false)));

  return Block::make(decls);
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
    auto node = arg.getNode<DataStructureArg>();
    auto ds = node->dsPtr();
    FERN_ASSERT(names.count(ds) > 0, "Should already have a name ready");
    if (node->meta) {
      return MetaData::make(Var::make(names[ds]), node->metaData);
    }
    return Var::make(names[ds]);
  }

  default:
    FERN_ASSERT(false, "Unreachable");
  }
}

Stmt CodeGenerator::generateComputeNode(const ComputeNode *node) const {

  std::cout << node->func << std::endl;

  std::vector<Expr> args;
  for (auto arg : node->func.getArguments()) {
    args.push_back(lowerArguments(arg, node->names));
  }
  return VoidCall::make(Call::make(node->func.getName(), args));
}

Stmt CodeGenerator::generatAllocateNode(const AllocateNode *node) const {
  std::vector<Expr> meta_data_vals;
  for (auto e : node->deps) {
    // Never want to put a declaration function arguments.
    meta_data_vals.push_back(generateExpr(e, {}, false));
  }

  auto var_decl =
      VarDecl::make(Type::make(node->ds->getTypeName()), node->name);
  auto func_call = Call::make(node->ds->getAllocData(), meta_data_vals);

  return VarAssign::make(var_decl, func_call);
}

Stmt CodeGenerator::generateInsertNode(const InsertNode *node) const {
  std::vector<Expr> meta_data_vals;
  for (auto e : node->deps) {
    // Never want to put a declaration function arguments.
    meta_data_vals.push_back(generateExpr(e, {}, false));
  }

  meta_data_vals.push_back(EscapeExpr::make(node->child));

  auto insert_call = Call::make(
      node->parent + "." + node->ds->getDataInsertInterface(), meta_data_vals);

  return VoidCall::make(insert_call);
}

Stmt CodeGenerator::generateFreeNode(const FreeNode *node) const {

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
  arg_vars.insert(cg.arg_vars.begin(), cg.arg_vars.end());
  return cg.getCode();
}

std::ostream &operator<<(std::ostream &os, const CodeGenerator &cg) {
  os << cg.getFullFunction();
  return os;
}

Stmt CodeGenerator::getFullFunction() const { return fullFunction; }

std::vector<Expr> CodeGenerator::generateFunctionHeaderArguments() {
  std::vector<Expr> args;
  for (auto input : pipeline.getTrueInputs()) {
    args.push_back(VarDecl::make(Type::make(input->getTypeName()),
                                 input->getVarName(), 1));
  }

  for (auto dummy : pipeline.getDummyDatastructures()) {
    args.push_back(VarDecl::make(Type::make(dummy->getTypeName()),
                                 dummy->getVarName(), dummy->isConst(),
                                 dummy->numRef(), dummy->numPtr()));
  }

  auto output = pipeline.getFinalOutput();
  args.push_back(
      VarDecl::make(Type::make(output->getTypeName()), output->getVarName()));

  for (const auto &undef : pipeline.getVariableArgs()) {
    // args.push_back(generateExpr(Variable(undef), {}, true));
    if (declared_var.count(undef) > 0) {
      continue;
    }

    arg_vars.insert(undef);
    declared_var.insert(undef);
  }
  // Get all the variables that have been left as undefined
  // util::printIterable(pipeline.undefined);
  auto interval_vars = pipeline.getIntervalVars();
  for (const auto &undef : pipeline.undefined) {
    if (declared_var.count(undef) > 0) {
      continue;
    }
    if (interval_vars.count(undef) > 0) {
      continue;
    }
    // args.push_back(generateExpr(Variable(undef), {}, true));
    arg_vars.insert(undef);
    declared_var.insert(undef);
  }

  for (const auto &to_declare : arg_vars) {
    args.push_back(generateExpr(Variable(to_declare), {}, true));
  }

  return args;
}

} // namespace codegen
} // namespace fern