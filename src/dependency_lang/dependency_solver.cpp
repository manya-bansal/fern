#include "dependency_lang/dependency_solver.h"

#include <ginac/ginac.h>

#include "dependency_lang/dep_lang_nodes.h"
#include "fern_error.h"
#include <sstream>

namespace fern {

std::ostream &operator<<(std::ostream &os, const solution &expr) {
  os << expr.var->name << "=" << expr.solution;

  return os;
}

// Given input and output dependencies of one datastructure, compute how these
// dependencies chain together. The first element of the array represents the
// last output, and so on.

// The arguments contained in the tuples should be of the same size
// expect potentially the last output (we are currently assuming that they are)
// the same data-structure.
void SolveDependency::solve() {
  // STEP 1: Populate a map of new names (should be disjoint) for each symbol in
  // the function annotations
  populate_names();
  // DEBUGGING SEE THE POPULATED NAMES
  // for (auto entry : names) {
  //     std::cout << (entry.first)->name << " is mapped to " << entry.second <<
  //     std::endl;
  // }

  // STEP 2: Generate the chained dependencies
  GiNaC::lst chained_deps;
  for (int i = 0; i < dependencies.size() - 1; i++) {
    auto input_func = get<0>(dependencies[i + 1]);
    auto output_func = get<1>(dependencies[i]);
    assert(input_func.size() == output_func.size());

    for (int i = 0; i < input_func.size(); i++) {
      auto input_ginac = generate_ginac(input_func[i]);
      auto output_ginac = generate_ginac(output_func[i]);
      // get the input symbols to the computation and solve for each symbol
      auto syms = input_func[i].getSymbols();
      for (auto sym : syms) {
        solutions.push_back(solution{
            .var = sym,
            .solution = lsolve(input_ginac == output_ginac, names[sym])});
      }
    }
  }

  std::cout << "verbose " << verbose << std::endl;
  // PRINT OUT SOLUTIONS FOR DEBUGGING
  if (verbose) {
    std::cout << "Solutions:" << std::endl;
    for (auto sol : solutions) {
      std::cout << DependencyExpr(sol.var) << " = " << sol.solution
                << std::endl;
      // Right now skip
      // convertToDependency(sol.solution);
    }
    std::cout << "-------------" << std::endl;
  }

  // Figure out the "free variables in the solutions"
  std::set<const DependencyVariableNode *> defined;
  for (auto sol : solutions) {
    defined.insert(sol.var);
  }

  for (auto entry : names) {
    if (defined.find(entry.first) == defined.end()) {
      undefined.push_back(entry.first);
    }
  }

  if (verbose) {
    std::cout << "Undefined :" << std::endl;
    for (auto undef : undefined) {
      std::cout << DependencyExpr(undef) << std::endl;
    }
    std::cout << "-------------" << std::endl;
  }
}

// COME BACK LATER, RN CAN GET AWAY WITH NOT CONVERTING BACK
// ADN JUST STORING AS A STRING
DependencySubset SolveDependency::convertToDependency(GiNaC::ex g) {
  struct get_ginac_symbols
      : public GiNaC::visitor,         // this is required
        public GiNaC::symbol::visitor, // visit symbol objects
        public GiNaC::add::visitor, // visit add objects (includes subtractions)
        public GiNaC::mul::visitor, // visit mul objects (includes division)
        public GiNaC::numeric::visitor // visit constants

  {
    get_ginac_symbols(
        std::map<GiNaC::symbol, const DependencyVariableNode *> symbol_mappings)
        : symbol_mappings(symbol_mappings) {}

    void visit(const GiNaC::symbol &x) {
      FERN_ASSERT(symbol_mappings.find(x) != symbol_mappings.end(),
                  "Symbol mapping not found");
      std::cout << "called with a symbol object " << x << std::endl;
      // symbols.insert(x);
    }

    void visit(const GiNaC::add &x) {
      for (int i = 0; i < x.nops(); i++) {
        auto op = x.op(i);
        // Get numeric coefficient
        // std::cout << x.op(i).coeff() << std::endl;
        // std::cout << x.op(i).coeff() << std::endl;
        // std::cout << x.get_var(i) << std::endl;
      }
      std::cout << "called with a add object " << x << std::endl;
      // symbols.insert(x);
    }

    void visit(const GiNaC::mul &x) {
      std::cout << "called with a mul object " << x << std::endl;
    }

    void visit(const GiNaC::numeric &x) {
      std::cout << "called with a numeric object " << x << std::endl;
    }

    std::map<GiNaC::symbol, const DependencyVariableNode *> symbol_mappings;
    DependencyExpr expr;
  };

  get_ginac_symbols v(names_reverse);
  g.accept(v);

  return DependencySubset();
}

std::set<const DependencyVariableNode *>
SolveDependency::list_of_unbound_variables(GiNaC::lst chained_deps) {
  struct GetSymbols : public DependencyVisitor {
    using DependencyVisitor::visit;
    std::set<const DependencyVariableNode *> get_symbols(DependencyExpr expr) {
      expr.accept(this);
      return symbols;
    }
    void visit(const DependencyVariableNode *node) { symbols.insert(node); }
    std::set<const DependencyVariableNode *> symbols;
  };

  std::set<const DependencyVariableNode *> undefined_symbols;

  // all of first output's are undefined
  for (auto expr : get<0>(dependencies[0])) {
    GetSymbols symbol_set;
    undefined_symbols = symbol_set.get_symbols(expr);
  }

  std::cout << "All the undefined symbols" << std::endl;
  for (auto var : undefined_symbols) {
    std::cout << var->name << " " << var->argument << std::endl;
  }

  return undefined_symbols;
}

GiNaC::ex SolveDependency::generate_ginac(DependencyExpr expr) {
  struct GenerateGiNaC : public DependencyExprVisitorStrict {
    using DependencyExprVisitorStrict::visit;
    GenerateGiNaC(std::map<const DependencyVariableNode *, GiNaC::symbol> names)
        : names(names) {}

    void generate_ginac(DependencyExpr expr) {
      GiNaC::ex new_ginac;
      GiNaC::ex ginac_expr = new_ginac;
      expr.accept(this);
    }

    void visit(const DependencyLiteralNode *op) {
      switch (op->getDataType().getKind()) {
      case Datatype::Int64:
        ginac_expr = op->getVal<int64_t>();
        break;
      case Datatype::Int32:
        ginac_expr = op->getVal<int32_t>();
        break;
      default:
        std::cerr << "Not supported datatype" << std::endl;
        break;
      }
    }
    void visit(const DependencyVariableNode *op) { ginac_expr = names[op]; }
    void visit(const AddNode *op) {
      op->a.accept(this);
      GiNaC::ex a_temp = ginac_expr;
      op->b.accept(this);
      GiNaC::ex b_temp = ginac_expr;
      ginac_expr = a_temp + b_temp;
    }

    void visit(const SubNode *op) {
      op->a.accept(this);
      GiNaC::ex a_temp = ginac_expr;
      op->b.accept(this);
      GiNaC::ex b_temp = ginac_expr;
      ginac_expr = a_temp - b_temp;
    }
    void visit(const DivNode *op) {
      op->a.accept(this);
      GiNaC::ex a_temp = ginac_expr;
      op->b.accept(this);
      GiNaC::ex b_temp = ginac_expr;
      ginac_expr = a_temp / b_temp;
    }
    void visit(const MulNode *op) {
      op->a.accept(this);
      GiNaC::ex a_temp = ginac_expr;
      op->b.accept(this);
      GiNaC::ex b_temp = ginac_expr;
      ginac_expr = a_temp * b_temp;
    }
    void visit(const ModNode *op) { std::cerr << "Not supported" << std::endl; }

    void visit(const MetaDataNode *op) {
      std::cout << MetaData(op) << std::endl;
      std::stringstream ss;
      ss << MetaData(op);
      ginac_expr = GiNaC::symbol(ss.str());
      // FERN_ASSERT(false, "unimplemented");
    }

    GiNaC::ex ginac_expr;
    std::map<const DependencyVariableNode *, GiNaC::symbol> names;
  };

  GenerateGiNaC ginac_gen(names);
  ginac_gen.generate_ginac(expr);
  return ginac_gen.ginac_expr;
}

void SolveDependency::populate_names() {
  std::map<const DependencyVariableNode *, GiNaC::symbol> names_temp;
  struct PopulateNames : public DependencyVisitor {
    using DependencyVisitor::visit;
    PopulateNames(
        std::map<const DependencyVariableNode *, GiNaC::symbol> &names_temp,
        const std::string &suffix)
        : names_temp(names_temp), suffix(suffix){};
    void populate_names(DependencyExpr expr) { expr.accept(this); }

    void visit(const DependencyVariableNode *node) {
      std::cout << "Pop names " << node->name << std::endl;
      // Just insert, it's fine if we replace!
      auto ginac_symbol = GiNaC::symbol(node->name + suffix);
      names_temp.insert({{node, ginac_symbol}});
      names_temp_rev.insert({{ginac_symbol, node}});
    }
    void set_suffix(std::string new_suffix) { suffix = new_suffix; }

    std::map<const DependencyVariableNode *, GiNaC::symbol> names_temp;
    std::map<GiNaC::symbol, const DependencyVariableNode *> names_temp_rev;
    std::string suffix;
  };

  PopulateNames pop_names(names_temp, "");
  for (int i = 0; i < dependencies.size(); i++) {
    // DEBUGGING: To check which function the name belongs to
    // pop_names.set_suffix("_func" + std::to_string(i));
    // Populate names for input
    for (auto expr : get<0>(dependencies[i])) {
      std::cout << expr << std::endl;
      pop_names.populate_names(expr);
    }
    // Populate names for output
    for (auto expr : get<1>(dependencies[i])) {
      std::cout << expr << std::endl;
      pop_names.populate_names(expr);
    }
  }

  names = pop_names.names_temp;
  names_reverse = pop_names.names_temp_rev;
}

} // namespace fern