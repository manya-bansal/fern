#ifndef PIPELINE_GRAPH_H
#define PIPELINE_GRAPH_H

#include <map>

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"

// This is a new alternate implementation of the pipeline builder that I had
// before, afraid that it doesn't work with all fo the reuse analysis.

namespace fern {
class DependencyGraph {
public:
  DependencyGraph(std::vector<ConcreteFunctionCall> func_calls);
  void print_node(std::ostream &os, int i, int ident, std::set<int> &visited,
                  std::map<int, std::set<int>> adj);
  void print(std::ostream &os);

  std::vector<int> get_children(int node_id);

  // This returns a list of nodes in last-to-first calling
  // order that contain forks. Order matters here!
  std::vector<int> identify_forks();

private:
  int getFuncIdx(ConcreteFunctionCall *f);

  std::map<int, ConcreteFunctionCall> idx;
  std::map<int, std::set<int>> childMap;
  // rn unclear whether this is actually useful, but I will build it :)
  std::map<int, std::set<int>> parentMap;

  // The root i.e. the last function call of our graph.
  ConcreteFunctionCall root;
  std::vector<ConcreteFunctionCall> func_calls;
};

std::ostream &operator<<(std::ostream &, DependencyGraph &);

} // namespace fern

#endif