#ifndef DEPENDENCY_SOLVER
#define DEPENDENCY_SOLVER

#include <ginac/ginac.h>

#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_visitor.h"

namespace fern {

struct solution {
  const DependencyVariableNode *var;
  GiNaC::ex solution;
};

std::ostream &operator<<(std::ostream &os, const solution &expr);

class SolveDependency {
public:
  SolveDependency(
      std::vector<
          std::tuple<std::vector<DependencyExpr>, std::vector<DependencyExpr>>>
          dependencies)
      : dependencies(dependencies) {
    assert(dependencies.size() >= 1);
    populate_names();
  };
  // Given input and output dependencies of one datastructure, compute how these
  // dependencies chain together.
  void solve();

  // print while generating
  void set_verbose() { verbose = true; }

  // These get populated after
  // solve() gets called
  std::vector<solution> solutions;
  std::vector<const DependencyVariableNode *> undefined;

private:
  void populate_names();
  std::vector<
      std::tuple<std::vector<DependencyExpr>, std::vector<DependencyExpr>>>
      dependencies;
  GiNaC::ex generate_ginac(DependencyExpr expr);
  std::set<const DependencyVariableNode *>
  list_of_unbound_variables(GiNaC::lst chained_deps);
  std::map<const DependencyVariableNode *, GiNaC::symbol> names;
  std::map<GiNaC::symbol, const DependencyVariableNode *> names_reverse;
  DependencySubset convertToDependency(GiNaC::ex g);
  bool verbose = false;
};

} // namespace fern

#endif