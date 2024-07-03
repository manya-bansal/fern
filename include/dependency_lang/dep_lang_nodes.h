#ifndef FERN_LANG_NODES
#define FERN_LANG_NODES

#include <string>

#include "assert.h"
#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes_abstract.h"
#include "dependency_lang/dep_lang_visitor.h"
#include "utils/instrusive_ptr.h"

namespace fern {

struct DependencyLiteralNode : public DependencyExprNode {
  template <typename T>
  explicit DependencyLiteralNode(T val) : DependencyExprNode(type<T>()) {
    this->val = malloc(sizeof(T));
    *static_cast<T *>(this->val) = val;
  }

  ~DependencyLiteralNode() { free(val); }

  void accept(DependencyExprVisitorStrict *v) const override { v->visit(this); }

  template <typename T> T getVal() const {
    if (getDataType() != type<T>()) {
      std::cerr << "Attempting to get data of wrong type" << std::endl;
    }

    return *static_cast<T *>(val);
  }

  void *val;
};

struct DependencyVariableNode : public DependencyExprNode {
  DependencyVariableNode() = delete;
  DependencyVariableNode(const std::string &name, bool argument = false,
                         bool parallelize = false, bool bound = false)
      : type(Datatype::Kind::Int64), name(name), argument(argument),
        parallelize(parallelize), bound(bound){};

  DependencyVariableNode(Datatype type, const std::string &name,
                         bool argument = false, bool parallelize = false,
                         bool bound = false)
      : type(type), name(name), argument(argument), parallelize(parallelize),
        bound(bound){};

  void accept(DependencyExprVisitorStrict *v) const { v->visit(this); }

  Datatype type;
  std::string name;
  bool argument;
  bool parallelize;
  // Tracks whether the variable is bound to an interval
  bool bound;
};

struct MetaDataNode : public DependencyExprNode {
  MetaDataNode() = delete;
  MetaDataNode(const std::string &name, const AbstractDataStructure *ds)
      : name(name), ds(ds){};

  void accept(DependencyExprVisitorStrict *v) const { v->visit(this); }

  std::string name;
  const AbstractDataStructure *ds;
};

struct BinaryExprNode : public DependencyExprNode {
  virtual std::string getOperatorString() const = 0;

  DependencyExpr a;
  DependencyExpr b;

protected:
  BinaryExprNode() : DependencyExprNode() {}
  BinaryExprNode(DependencyExpr a, DependencyExpr b)
      : DependencyExprNode(max_type(a.getDataType(), b.getDataType())), a(a),
        b(b) {}
};

struct AddNode : public BinaryExprNode {
  AddNode() : BinaryExprNode() {}
  AddNode(DependencyExpr a, DependencyExpr b) : BinaryExprNode(a, b) {}

  std::string getOperatorString() const { return "+"; }

  void accept(DependencyExprVisitorStrict *v) const { v->visit(this); }
};

struct SubNode : public BinaryExprNode {
  SubNode() : BinaryExprNode() {}
  SubNode(DependencyExpr a, DependencyExpr b) : BinaryExprNode(a, b) {}

  std::string getOperatorString() const { return "-"; }

  void accept(DependencyExprVisitorStrict *v) const { v->visit(this); }
};

struct MulNode : public BinaryExprNode {
  MulNode() : BinaryExprNode() {}
  MulNode(DependencyExpr a, DependencyExpr b) : BinaryExprNode(a, b) {}

  std::string getOperatorString() const { return "*"; }

  void accept(DependencyExprVisitorStrict *v) const { v->visit(this); }
};

struct DivNode : public BinaryExprNode {
  DivNode() : BinaryExprNode() {}
  DivNode(DependencyExpr a, DependencyExpr b) : BinaryExprNode(a, b) {}

  std::string getOperatorString() const { return "/"; }

  void accept(DependencyExprVisitorStrict *v) const { v->visit(this); }
};

struct ModNode : public BinaryExprNode {
  ModNode() : BinaryExprNode() {}
  ModNode(DependencyExpr a, DependencyExpr b) : BinaryExprNode(a, b) {}

  std::string getOperatorString() const { return "%"; }

  void accept(DependencyExprVisitorStrict *v) const { v->visit(this); }
};

struct IntervalNode : public DependencySubsetNode {
  IntervalNode() = delete;
  IntervalNode(Variable var, DependencyExpr start, DependencyExpr end,
               DependencyExpr step, bool recompute = false)
      : var(var), start(start), end(end), step(step), recompute(recompute) {
    // Need to rename
    FERN_ASSERT(var.isBound(), "Interval Variable must be "
                               "bound");
  }
  IntervalNode(Variable var, DependencyExpr start, DependencyExpr end,
               DependencyExpr step, DependencySubset child,
               bool recompute = false)
      : var(var), start(start), end(end), step(step), child(child),
        recompute(recompute) {
    FERN_ASSERT(var.isBound(), "Interval Variable must be "
                               "bound");
  }

  void accept(DependencySubsetVisitorStrict *v) const { v->visit(this); }

  Variable var;
  DependencyExpr start;
  DependencyExpr end;
  DependencyExpr step;

  DependencySubset child;

  // Recomputation is set as false by default
  bool recompute = false;
};

struct DataStructureNode : public DependencySubsetNode {
  DataStructureNode() = delete;
  DataStructureNode(const std::string &name,
                    const AbstractDataStructure *abstractData)
      : name(name), abstractData(abstractData){};
  DataStructureNode(const std::string &name,
                    const AbstractDataStructure *abstractData,
                    std::vector<DependencyExpr> annotations)
      : name(name), abstractData(abstractData), annotations(annotations){};

  void accept(DependencySubsetVisitorStrict *v) const { v->visit(this); }
  std::string name;
  const AbstractDataStructure *abstractData;
  std::vector<DependencyExpr> annotations;
};

struct ProducerNode : public DependencySubsetNode {
  ProducerNode() = delete;
  ProducerNode(DependencySubset child) : child(child) {}

  void accept(DependencySubsetVisitorStrict *v) const { v->visit(this); }

  DependencySubset child;
};

struct ConsumerNode : public DependencySubsetNode {
  ConsumerNode() = delete;
  ConsumerNode(const std::vector<DependencySubset> &child) : child(child) {}

  void accept(DependencySubsetVisitorStrict *v) const { v->visit(this); }

  // can require multiple arguments, so use
  // a vector
  std::vector<DependencySubset> child;
};

struct ComputationAnnotationNode : public DependencySubsetNode {
  ComputationAnnotationNode() = delete;
  ComputationAnnotationNode(Producer producer, Consumer consumer)
      : producer(producer), consumer(consumer) {}

  void accept(DependencySubsetVisitorStrict *v) const { v->visit(this); }

  Producer producer;
  Consumer consumer;
};

// struct SubsetNode : public DependencySubsetNode {
//     SubsetNode() = delete;
//     SubsetNode(MetaData meta, DependencyExpr expr, enum COMPARISON_OP op) :
//     meta(meta), expr(expr), op(op){};

//     void accept(DependencySubsetVisitorStrict* v) const {
//         v->visit(this);
//     }

//     MetaData meta;
//     DependencyExpr expr;
//     enum COMPARISON_OP op;
// };

/// Returns true if expression e is of type E.
template <typename E> inline bool isa(const DependencyExprNode *e) {
  return e != nullptr && dynamic_cast<const E *>(e) != nullptr;
};

template <typename E> inline const E *to(const DependencyExprNode *e) {
  assert(isa<E>(e));
  //  << "Cannot convert " << typeid(e).name() << " to " << typeid(E).name();
  return static_cast<const E *>(e);
}

template <typename I> inline const typename I::Node *getNode(const I &stmt) {
  assert(isa<typename I::Node>(stmt.ptr));
  return static_cast<const typename I::Node *>(stmt.ptr);
};

/// Returns true if expression e is of type E.
template <typename E> inline bool isa(const DependencySubsetNode *e) {
  return e != nullptr && dynamic_cast<const E *>(e) != nullptr;
};

template <typename E> inline const E *to(const DependencySubsetNode *e) {
  assert(isa<E>(e));
  //  << "Cannot convert " << typeid(e).name() << " to " << typeid(E).name();
  return static_cast<const E *>(e);
}

/// Returns true if expression e is of type E.
template <typename E> inline bool isa(const DependencyIterNode *e) {
  return e != nullptr && dynamic_cast<const E *>(e) != nullptr;
};

template <typename E> inline const E *to(const DependencyIterNode *e) {
  assert(isa<E>(e));
  //  << "Cannot convert " << typeid(e).name() << " to " << typeid(E).name();
  return static_cast<const E *>(e);
}

} // namespace fern

#endif