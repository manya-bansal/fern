#include "dependency_lang/dep_lang_rewriter.h"

#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"

namespace fern {

// class ExprRewriterStrict
DependencyExpr DependencyExprRewriterStrict::rewrite(DependencyExpr e) {
  if (e.defined()) {
    e.accept(this);
    e = expr;
  } else {
    e = DependencyExpr();
  }
  expr = DependencyExpr();
  return e;
}

DependencySubset DependencySubsetRewriterStrict::rewrite(DependencySubset s) {
  if (s.defined()) {
    s.accept(this);
    s = subset;
  } else {
    s = DependencySubset();
  }
  subset = DependencySubset();
  return s;
}

template <class T>
DependencyExpr visitBinaryOp(const T *op, DependencyRewriter *rw) {
  DependencyExpr a = rw->rewrite(op->a);
  DependencyExpr b = rw->rewrite(op->b);
  if (a == op->a && b == op->b) {
    return DependencyExpr(op);
  } else {
    return DependencyExpr(new T(a, b));
  }
}

void DependencyRewriter::visit(const DependencyLiteralNode *op) {
  expr = DependencyExpr(op);
}

void DependencyRewriter::visit(const DependencyVariableNode *op) {
  expr = DependencyExpr(op);
}

//  void DependencyRewriter::visit(const DependencyArgumentNode* op) = 0;
void DependencyRewriter::visit(const AddNode *op) {
  expr = visitBinaryOp(op, this);
}

void DependencyRewriter::visit(const SubNode *op) {
  expr = visitBinaryOp(op, this);
}

void DependencyRewriter::visit(const DivNode *op) {
  expr = visitBinaryOp(op, this);
}

void DependencyRewriter::visit(const MulNode *op) {
  expr = visitBinaryOp(op, this);
}

void DependencyRewriter::visit(const ModNode *op) {
  expr = visitBinaryOp(op, this);
}

void DependencyRewriter::visit(const MetaDataNode *op) {
  expr = DependencyExpr(op);
}

// void DependencyRewriter::visit(const SubsetNode* op) {
//     DependencyExpr a = rewrite(op->expr);
//     if (a == op->expr) {
//         subset = DependencySubset(op);
//     } else {
//         subset = DependencySubset(new SubsetNode(op->meta, a, op->op));
//     }
// }

void DependencyRewriter::visit(const DataStructureNode *op) {
  std::vector<DependencyExpr> rw_annot;
  for (auto annot : op->annotations) {
    rw_annot.push_back(rewrite(annot));
  }

  subset = DependencySubset(
      new DataStructureNode(op->name, op->abstractData, rw_annot));
}

void DependencyRewriter::visit(const IntervalNode *op) {
  DependencyExpr rw_start = rewrite(op->start);
  DependencyExpr rw_end = rewrite(op->end);
  DependencyExpr rw_step = rewrite(op->step);
  Variable rw_var = to<Variable>(rewrite(op->var));

  DependencySubset rw_child = rewrite(op->child);

  subset = DependencySubset(new IntervalNode(rw_var, rw_start, rw_end, rw_step,
                                             rw_child, op->recompute));
}

void DependencyRewriter::visit(const ProducerNode *op) {
  DependencySubset rw_child = rewrite(op->child);
  subset = Producer(rw_child);
}

void DependencyRewriter::visit(const ConsumerNode *op) {
  std::vector<DependencySubset> rw_annot;
  for (auto annot : op->child) {
    rw_annot.push_back(rewrite(annot));
  }
  subset = Consumer(rw_annot);
}

void DependencyRewriter::visit(const ComputationAnnotationNode *op) {
  Producer rw_prod = to<Producer>(rewrite(op->producer));
  Consumer rw_cons = to<Consumer>(rewrite(op->consumer));

  subset = ComputationAnnotation(rw_prod, rw_cons);
}

DependencySubset ChangeVariableNames(
    DependencySubset s,
    std::map<const DependencyVariableNode *, const DependencyVariableNode *>
        replace) {
  struct ReplaceRewriter : public DependencyRewriter {
    using DependencyRewriter::visit;
    ReplaceRewriter(
        std::map<const DependencyVariableNode *, const DependencyVariableNode *>
            replace)
        : replace(replace) {}

    void visit(const DependencyVariableNode *op) {
      if (replace.find(op) != replace.end()) {
        expr = DependencyExpr(replace[op]);
      } else {
        expr = DependencyExpr(op);
      }
    }

    std::map<const DependencyVariableNode *, const DependencyVariableNode *>
        replace;
  };

  ReplaceRewriter rw(replace);
  auto new_s = rw.rewrite(s);

  return new_s;
}

void FlattenExpr::visit(const DependencyLiteralNode *op) {
  nodeTypes.push_back(LITERAL);
  nodes.push_back(DependencyExpr(op));
}
void FlattenExpr::visit(const DependencyVariableNode *op) {
  nodeTypes.push_back(VAR);
  nodes.push_back(DependencyExpr(op));
}
void FlattenExpr::visit(const AddNode *op) {
  nodeTypes.push_back(ADD);
  op->a.accept(this);
  op->b.accept(this);
}
void FlattenExpr::visit(const SubNode *op) {
  nodeTypes.push_back(SUB);
  op->a.accept(this);
  op->b.accept(this);
}
void FlattenExpr::visit(const DivNode *op) {
  nodeTypes.push_back(DIV);
  op->a.accept(this);
  op->b.accept(this);
}
void FlattenExpr::visit(const MulNode *op) {
  nodeTypes.push_back(MUL);
  op->a.accept(this);
  op->b.accept(this);
}
void FlattenExpr::visit(const ModNode *op) {
  nodeTypes.push_back(MOD);
  op->a.accept(this);
  op->b.accept(this);
}

void FlattenExpr::visit(const MetaDataNode *op) {
  nodeTypes.push_back(META_DATA);
  nodes.push_back(DependencyExpr(op));
}

bool check_abstract_equality(std::vector<DependencyExpr> e1,
                             std::vector<DependencyExpr> e2) {

  // We will go step by step
  if (e1.size() != e2.size()) {
    return false;
  }

  std::map<const DependencyVariableNode *, const DependencyVariableNode *>
      var_mappings;
  std::map<const AbstractDataStructure *, const AbstractDataStructure *>
      ds_mappings;

  for (int i = 0; i < e1.size(); i++) {
    auto e1_cur = e1[i];
    auto e2_cur = e2[i];

    // Flatten e1
    FlattenExpr e1_flattener{};
    e1_flattener.visit(e1_cur);
    auto e1_node_type = e1_flattener.nodeTypes;
    auto e1_nodes = e1_flattener.nodes;

    // Flatten e2
    FlattenExpr e2_flattener{};
    e2_flattener.visit(e2_cur);
    auto e2_node_type = e2_flattener.nodeTypes;
    auto e2_nodes = e2_flattener.nodes;

    // Make sure that the nodesTypes match
    if (e1_node_type.size() != e2_node_type.size()) {
      std::cout << "node type size don't match" << std::endl;
      return false;
    }

    int node_idx = 0;

    for (int n = 0; n < e1_node_type.size(); n++) {
      // Now check that the node types are the same
      if (e1_node_type[n] != e2_node_type[n]) {
        return false;
      }

      // check that the concrete value matches up
      switch (e1_node_type[n]) {
      case FlattenExpr::VAR: {
        auto v1 = getNode(to<Variable>(e1_nodes[node_idx]));
        auto v2 = getNode(to<Variable>(e2_nodes[node_idx]));
        if (var_mappings.find(v1) == var_mappings.end()) {
          // insert  a mapping that we will check against
          var_mappings[v1] = v2;
        }
        if (var_mappings[v1] != v2) {
          return false;
        }
        node_idx++;
      }; break;

      case FlattenExpr::LITERAL: {
        if (e1_nodes[node_idx].getDataType() !=
            e2_nodes[node_idx].getDataType()) {
          return false;
        }

        // Very bad and hacky, but just comparing strings =p
        std::stringstream v1;
        std::stringstream v2;

        v1 << e1_nodes[node_idx];
        v2 << e2_nodes[node_idx];

        if (v1.str() != v2.str()) {
          return false;
        }

        node_idx++;

      }; break;

      case FlattenExpr::META_DATA: {
        // Can compare the strings again?
        auto v1 = to<MetaData>(e1_nodes[node_idx]);
        auto v2 = to<MetaData>(e2_nodes[node_idx]);

        if (v1.getName() != v2.getName()) {
          return false;
        }

        auto v1_ds = v1.getDataStructure();
        auto v2_ds = v2.getDataStructure();

        if (ds_mappings.find(v1_ds) == ds_mappings.end()) {
          // insert  a mapping that we will check against
          ds_mappings[v1_ds] = v2_ds;
        }
        if (ds_mappings[v1_ds] != v2_ds) {
          return false;
        }
        node_idx++;
      }; break;

      default:
        break;
      }
    }
  }

  return true;
}

} // namespace fern
