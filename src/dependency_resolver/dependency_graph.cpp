#include "dependency_resolver/dependency_graph.h"

#include <map>

namespace fern {
DependencyGraph::DependencyGraph(std::vector<ConcreteFunctionCall> func_calls)
    : root{}, func_calls(func_calls) {

  // 0 is always the root. Root is a ghost node that I insert.
  idx[0] = root;
  // Allocate an idx to every function call
  for (int i = 0; i < func_calls.size(); i++) {
    idx[i + 1] = func_calls[i];
  }

  // Initialize the root, we will definitely need it!
  childMap[0] = std::set<int>{};

  // What datastructure is produced by what function to keep track of
  // dependencies between function call order.
  std::map<const AbstractDataStructure *, int> computed_from;

  for (int i = 0; i < func_calls.size(); i++) {
    auto func = func_calls[i];
    // The true
    auto getOutput = func.getOutput();
    computed_from[getOutput] = i + 1;
    // Go over all of the inputs, and see whether they are being
    // computed from some prevoius function

    bool dependent = false;

    for (auto input : func.getInputs()) {
      if (computed_from.find(input) != computed_from.end()) {
        int parent_func = computed_from[input];
        dependent = true;
        if (childMap.find(parent_func) == childMap.end()) {
          childMap[parent_func] = std::set<int>();
        }
        childMap[parent_func].insert(i + 1);
        std::cout << "Inserting @ " << parent_func << " =  " << i + 1
                  << std::endl;
      }
    }

    // Then add it to the root map if it is dependent on nothing
    if (!dependent) {
      childMap[0].insert(i + 1);
    }
  }

  // Now flip to generate the parent map

  for (auto rel : childMap) {
    for (int child : rel.second) {
      if (parentMap.find(child) == parentMap.end()) {
        parentMap[child] = std::set<int>();
      }
      parentMap[child].insert(rel.first);
    }
  }
}

int DependencyGraph::getFuncIdx(ConcreteFunctionCall *f) {

  if (f == &root) {
    return 0;
  }

  for (int i = 0; i < func_calls.size(); i++) {
    if (&(func_calls[i]) == f) {
      return i + 1;
    }
  }

  // should never happen
  return -1;
}

static void print_ident(std::ostream &os, int ident_level) {
  for (int i = 0; i < ident_level; i++) {
    os << '\t';
  }
}

void DependencyGraph::print_node(std::ostream &os, int i, int ident,
                                 std::set<int> &visited,
                                 std::map<int, std::set<int>> adj) {

  FERN_ASSERT_NO_MSG(idx.find(i) != idx.end());
  visited.insert(i);

  auto children = adj[i];
  for (auto child : children) {
    if (visited.find(child) == visited.end()) {
      print_ident(os, ident);
      os << i << " : " << child << "  " << idx[child] << " --->" << std::endl;
    }
  }

  for (auto child : children) {
    if (visited.find(child) == visited.end()) {
      print_node(os, child, ident++, visited, adj);
    }
  }

  ident--;
}

std::vector<int> DependencyGraph::identify_forks() {
  // return a list of all the functions (bottom up)
  // that have forks so that we can handle

  std::vector<int> fork_nodes;
  int size_of_func = func_calls.size();
  for (int f = size_of_func; f > 0; f--) {
    // function may not have a child
    if (childMap.find(f) != childMap.end()) {
      if (childMap[f].size() > 1) {
        fork_nodes.push_back(f);
      }
    }
  }

  return fork_nodes;
}

std::vector<int> DependencyGraph::get_children(int node_id) {
  // return a list of all the functions (bottom up)
  // that have forks so that we can handle

  std::vector<int> children;

  // function may not have a child
  if (childMap.find(node_id) != childMap.end()) {
    auto children_set = childMap[node_id];
    for (auto &child : children_set) {
      children.push_back(child);
    }
  }

  return children;
}

void DependencyGraph::print(std::ostream &os) {
  std::set<int> visited;
  os << "0 : ROOT << "
     << "Child Map"
     << "--->" << std::endl;

  // for (auto child : childMap[0]) {
  print_node(os, 0, 1, visited, childMap);
  // }

  for (auto rel : parentMap) {
    for (auto parent : rel.second) {
      std::cout << rel.first << " : " << parent << std::endl;
    }
  }
}

std::ostream &operator<<(std::ostream &os, DependencyGraph &p) {
  p.print(os);
  return os;
}
} // namespace fern