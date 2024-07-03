#include "dependency_lang/dep_lang.h"

#include <cassert>
#include <set>

#include "dependency_lang/dep_lang_nodes.h"
#include "dependency_lang/dep_lang_printer.h"
#include "dependency_lang/dep_lang_rewriter.h"
#include "fern_error.h"

namespace fern {

DependencyExpr::DependencyExpr(char val)
    : DependencyExpr(new DependencyLiteralNode(val)) {}

DependencyExpr::DependencyExpr(std::string name, bool argument)
    : DependencyExpr(new DependencyVariableNode(name, argument)) {}

DependencyExpr::DependencyExpr(int8_t val)
    : DependencyExpr(new DependencyLiteralNode(val)) {}

DependencyExpr::DependencyExpr(int16_t val)
    : DependencyExpr(new DependencyLiteralNode(val)) {}

DependencyExpr::DependencyExpr(int32_t val)
    : DependencyExpr(new DependencyLiteralNode(val)) {}

DependencyExpr::DependencyExpr(int64_t val)
    : DependencyExpr(new DependencyLiteralNode(val)) {}

DependencyExpr::DependencyExpr(uint8_t val)
    : DependencyExpr(new DependencyLiteralNode(val)) {}

DependencyExpr::DependencyExpr(uint16_t val)
    : DependencyExpr(new DependencyLiteralNode(val)) {}

DependencyExpr::DependencyExpr(uint32_t val)
    : DependencyExpr(new DependencyLiteralNode(val)) {}

DependencyExpr::DependencyExpr(uint64_t val)
    : DependencyExpr(new DependencyLiteralNode(val)) {}

DependencyExpr::DependencyExpr(float val)
    : DependencyExpr(new DependencyLiteralNode(val)) {}

DependencyExpr::DependencyExpr(double val)
    : DependencyExpr(new DependencyLiteralNode(val)) {}

Datatype DependencyExpr::getDataType() const {
  return const_cast<DependencyExprNode *>(this->ptr)->getDataType();
}

std::set<const DependencyVariableNode *> DependencyExpr::getSymbols() const {
  struct GetSymbols : public DependencyVisitor {
    using DependencyVisitor::visit;
    std::set<const DependencyVariableNode *> get_symbols(DependencyExpr expr) {
      expr.accept(this);
      return symbols;
    }
    void visit(const DependencyVariableNode *node) { symbols.insert(node); }
    std::set<const DependencyVariableNode *> symbols;
  };

  GetSymbols symbol_set;
  return symbol_set.get_symbols(*this);
}

void DependencyExpr::accept(DependencyExprVisitorStrict *v) const {
  if (!defined()) {
    return;
  }
  ptr->accept(v);
}

std::ostream &operator<<(std::ostream &os, const DependencyExpr &expr) {
  if (!expr.defined())
    return os << "DependencyExpr()";
  DependencyPrinter printer(os);
  printer.print(expr);
  return os;
}

DependencyExpr operator+(const DependencyExpr &lhs, const DependencyExpr &rhs) {
  return DependencyExpr(new AddNode(lhs, rhs));
}

DependencyExpr operator-(const DependencyExpr &lhs, const DependencyExpr &rhs) {
  return DependencyExpr(new SubNode(lhs, rhs));
}

DependencyExpr operator*(const DependencyExpr &lhs, const DependencyExpr &rhs) {
  return DependencyExpr(new MulNode(lhs, rhs));
}

DependencyExpr operator/(const DependencyExpr &lhs, const DependencyExpr &rhs) {
  return DependencyExpr(new DivNode(lhs, rhs));
}

DependencyExpr operator%(const DependencyExpr &lhs, const DependencyExpr &rhs) {
  return DependencyExpr(new ModNode(lhs, rhs));
}

DependencyLiteral::DependencyLiteral(const DependencyLiteralNode *n)
    : DependencyExpr(n) {}

DependencyLiteral::DependencyLiteral(int64_t val)
    : DependencyExpr(new DependencyLiteralNode(val)) {}

DependencyLiteral::DependencyLiteral(int val)
    : DependencyExpr(new DependencyLiteralNode(val)) {}

template <typename T> T DependencyLiteral::getVal() const {
  return getNode(*this)->getVal<T>();
}

void *DependencyLiteral::getValPtr() { return getNode(*this)->val; }

Variable::Variable(const DependencyVariableNode *n) : DependencyExpr(n) {}

Variable::Variable(const std::string &name, bool argument, bool parallelize,
                   bool bound)
    : DependencyExpr(
          new DependencyVariableNode(name, argument, parallelize, bound)) {}

std::string Variable::getName() const { return getNode(*this)->name; }

Datatype Variable::getType() const { return getNode(*this)->type; }

bool Variable::isArgument() const { return getNode(*this)->argument; }

bool Variable::isParallel() const { return getNode(*this)->parallelize; }

bool Variable::isBound() const { return getNode(*this)->bound; }

template <> bool isa<Variable>(DependencyExpr e) {
  return isa<DependencyVariableNode>(e.ptr);
}

template <> Variable to<Variable>(DependencyExpr e) {
  assert(isa<Variable>(e));
  return Variable(to<DependencyVariableNode>(e.ptr));
}

// Argument::Argument(const DependencyArgumentNode* n) : DependencyExpr(n) {
// }

// Argument::Argument(const std::string& name) : DependencyExpr(new
// DependencyArgumentNode(name)) {
// }

// std::string Argument::getName() const {
//     return getNode(*this)->name;
// }

// template <>
// bool isa<Argument>(DependencyExpr e) {
//     return isa<DependencyArgumentNode>(e.ptr);
// }

// template <>
// Argument to<Argument>(DependencyExpr e) {
//     assert(isa<Argument>(e));
//     return Argument(to<DependencyArgumentNode>(e.ptr));
// }

Add::Add() : Add(new AddNode) {}

Add::Add(const AddNode *n) : DependencyExpr(n) {}

Add::Add(DependencyExpr a, DependencyExpr b) : Add(new AddNode(a, b)) {}

DependencyExpr Add::getA() const { return getNode(*this)->a; }

DependencyExpr Add::getB() const { return getNode(*this)->b; }

template <> bool isa<Add>(DependencyExpr e) { return isa<AddNode>(e.ptr); }

template <> Add to<Add>(DependencyExpr e) {
  assert(isa<Add>(e));
  return Add(to<AddNode>(e.ptr));
}

Sub::Sub() : Sub(new SubNode) {}

Sub::Sub(const SubNode *n) : DependencyExpr(n) {}

Sub::Sub(DependencyExpr a, DependencyExpr b) : Sub(new SubNode(a, b)) {}

DependencyExpr Sub::getA() const { return getNode(*this)->a; }

DependencyExpr Sub::getB() const { return getNode(*this)->b; }

template <> bool isa<Sub>(DependencyExpr e) { return isa<SubNode>(e.ptr); }

template <> Sub to<Sub>(DependencyExpr e) {
  assert(isa<Sub>(e));
  return Sub(to<SubNode>(e.ptr));
}

Mul::Mul() : Mul(new MulNode) {}

Mul::Mul(const MulNode *n) : DependencyExpr(n) {}

Mul::Mul(DependencyExpr a, DependencyExpr b) : Mul(new MulNode(a, b)) {}

DependencyExpr Mul::getA() const { return getNode(*this)->a; }

DependencyExpr Mul::getB() const { return getNode(*this)->b; }

template <> bool isa<Mul>(DependencyExpr e) { return isa<MulNode>(e.ptr); }

template <> Mul to<Mul>(DependencyExpr e) {
  assert(isa<Mul>(e));
  return Mul(to<MulNode>(e.ptr));
}

Div::Div() : Div(new DivNode) {}

Div::Div(const DivNode *n) : DependencyExpr(n) {}

Div::Div(DependencyExpr a, DependencyExpr b) : Div(new DivNode(a, b)) {}

DependencyExpr Div::getA() const { return getNode(*this)->a; }

DependencyExpr Div::getB() const { return getNode(*this)->b; }

template <> bool isa<Div>(DependencyExpr e) { return isa<DivNode>(e.ptr); }

template <> Div to<Div>(DependencyExpr e) {
  assert(isa<Div>(e));
  return Div(to<DivNode>(e.ptr));
}

Mod::Mod() : Mod(new ModNode) {}

Mod::Mod(const ModNode *n) : DependencyExpr(n) {}

Mod::Mod(DependencyExpr a, DependencyExpr b) : Mod(new ModNode(a, b)) {}

DependencyExpr Mod::getA() const { return getNode(*this)->a; }

DependencyExpr Mod::getB() const { return getNode(*this)->b; }

template <> bool isa<Mod>(DependencyExpr e) { return isa<ModNode>(e.ptr); }

template <> Mod to<Mod>(DependencyExpr e) {
  assert(isa<Mod>(e));
  return Mod(to<ModNode>(e.ptr));
}

MetaData::MetaData(const MetaDataNode *n) : DependencyExpr(n) {}

MetaData::MetaData(const std::string &name, const AbstractDataStructure *ds)
    : MetaData(new MetaDataNode(name, ds)) {}

std::string MetaData::getName() { return getNode(*this)->name; }

const AbstractDataStructure *MetaData::getDataStructure() {
  return getNode(*this)->ds;
}

template <> bool isa<MetaData>(DependencyExpr e) {
  return isa<MetaDataNode>(e.ptr);
}

template <> MetaData to<MetaData>(DependencyExpr e) {
  assert(isa<MetaData>(e));
  return MetaData(to<MetaDataNode>(e.ptr));
}

void DependencyIter::accept(DependencyIterVisitorStrict *v) const {
  ptr->accept(v);
}

std::ostream &operator<<(std::ostream &os, const DependencyIter &expr) {
  if (!expr.defined())
    return os << "DependencySubset()";
  DependencyPrinter printer(os);
  printer.print(expr);
  return os;
}

Interval::Interval(const IntervalNode *n) : DependencySubset(n) {}

Interval::Interval(Variable var, DependencyExpr start, DependencyExpr end,
                   DependencyExpr step, bool recompute)
    : DependencySubset(new IntervalNode(var, start, end, step, recompute)) {}

Interval::Interval(Variable var, DependencyExpr start, DependencyExpr end,
                   DependencyExpr step, DependencySubset child, bool recompute)
    : DependencySubset(
          new IntervalNode(var, start, end, step, child, recompute)) {}

DependencyExpr Interval::getStart() const { return getNode(*this)->start; }

DependencyExpr Interval::getEnd() const { return getNode(*this)->end; }

DependencyExpr Interval::getStep() const { return getNode(*this)->step; }

DependencySubset Interval::getChild() const { return getNode(*this)->child; }

Variable Interval::getVar() const { return getNode(*this)->var; }

bool Interval::isRecompute() const { return getNode(*this)->recompute; }

template <> bool isa<Interval>(DependencySubset e) {
  return isa<IntervalNode>(e.ptr);
}

template <> Interval to<Interval>(DependencySubset e) {
  assert(isa<Interval>(e));
  return Interval(to<IntervalNode>(e.ptr));
}

Producer::Producer(const ProducerNode *n) : DependencySubset(n) {}

Producer::Producer(DependencySubset child)
    : DependencySubset(new ProducerNode(child)) {}

DependencySubset Producer::getChild() const { return getNode(*this)->child; }

template <> bool isa<Producer>(DependencySubset e) {
  return isa<ProducerNode>(e.ptr);
}

template <> Producer to<Producer>(DependencySubset e) {
  assert(isa<Producer>(e));
  return Producer(to<ProducerNode>(e.ptr));
}

Consumer::Consumer(const ConsumerNode *n) : DependencySubset(n) {}

Consumer::Consumer(const std::vector<DependencySubset> &child)
    : DependencySubset(new ConsumerNode(child)) {}

std::vector<DependencySubset> Consumer::getChild() const {
  return getNode(*this)->child;
}

template <> bool isa<Consumer>(DependencySubset e) {
  return isa<ConsumerNode>(e.ptr);
}

template <> Consumer to<Consumer>(DependencySubset e) {
  assert(isa<Consumer>(e));
  return Consumer(to<ConsumerNode>(e.ptr));
}

ComputationAnnotation::ComputationAnnotation(const ComputationAnnotationNode *n)
    : DependencySubset(n) {}
ComputationAnnotation::ComputationAnnotation(Producer producer,
                                             Consumer consumer)
    : DependencySubset(new ComputationAnnotationNode(producer, consumer)) {}

Producer ComputationAnnotation::getProducer() const {
  return getNode(*this)->producer;
}

Consumer ComputationAnnotation::getConsumer() const {
  return getNode(*this)->consumer;
}

template <> bool isa<ComputationAnnotation>(DependencySubset e) {
  return isa<ComputationAnnotationNode>(e.ptr);
}

template <>
ComputationAnnotation to<ComputationAnnotation>(DependencySubset e) {
  assert(isa<ComputationAnnotation>(e));
  return ComputationAnnotation(to<ComputationAnnotationNode>(e.ptr));
}

void DependencySubset::accept(DependencySubsetVisitorStrict *v) const {
  if (!defined()) {
    return;
  }
  ptr->accept(v);
}

std::ostream &operator<<(std::ostream &os, const DependencySubset &expr) {
  if (!expr.defined())
    return os << "DependencySubset()";
  DependencyPrinter printer(os);
  printer.print(expr);
  return os;
}

DataStructure::DataStructure(const DataStructureNode *n)
    : DependencySubset(n) {}

DataStructure::DataStructure(const std::string &name,
                             const AbstractDataStructure *abstractData)
    : DataStructure(new DataStructureNode(name, abstractData)) {}

DataStructure::DataStructure(const std::string &name,
                             const AbstractDataStructure *abstractData,
                             std::vector<DependencyExpr> annotations)
    : DataStructure(new DataStructureNode(name, abstractData, annotations)) {}

std::string DataStructure::getName() const { return getNode(*this)->name; }

// DataStructure DataStructure::operator()(DependencyExpr expr) {
//     std::vector<DependencyExpr> annot;
//     annot.push_back(expr);
//     return DataStructure(getName(), getAbstractDataStructure(), annot);
// }

// template <typename FirstT, typename... Args>
// DataStructure DataStructure::operator()(FirstT first, Args... remaining) {
//     std::vector<DependencyExpr> annot;
//     addAnnot(annot, first, remaining...);
//     // Ensure that the size of the meta data fields and the annotations lines
//     // up
//     assert(annot.size() == getAbstractDataStructure()->getMetaData().size());
//     return DataStructure(getName(), getAbstractDataStructure(), annot);
// }

const AbstractDataStructure *DataStructure::getAbstractDataStructure() const {
  return getNode(*this)->abstractData;
}

std::vector<DependencyExpr> DataStructure::getAnnotations() const {
  return getNode(*this)->annotations;
}

template <> bool isa<DataStructure>(DependencySubset e) {
  return isa<DataStructureNode>(e.ptr);
}

template <> DataStructure to<DataStructure>(DependencySubset e) {
  assert(isa<DataStructure>(e));
  return DataStructure(to<DataStructureNode>(e.ptr));
}

std::set<const DependencyVariableNode *> DependencySubset::getVariables() {
  struct GetSymbols : public DependencyVisitor {
    using DependencyVisitor::visit;
    std::set<const DependencyVariableNode *>
    get_symbols(DependencySubset expr) {
      std::cout << "yolo" << std::endl;
      expr.accept(this);
      std::cout << "yikes" << std::endl;
      return symbols;
    }
    void visit(const DependencyVariableNode *node) { symbols.insert(node); }
    std::set<const DependencyVariableNode *> symbols;
  };

  GetSymbols symbol_set;
  std::cout << "out help" << std::endl << *this << std::endl;
  return symbol_set.get_symbols(*this);
}

// Scheduling Commands

DependencySubset DependencySubset::split(Variable old, Variable outer,
                                         Variable inner, int split_param) {
  struct ReplaceRewriter : public DependencyRewriter {
    using DependencyRewriter::visit;

    ReplaceRewriter(Variable old, Variable outer, Variable inner,
                    int64_t split_param)
        : old(old), outer(outer), inner(inner), split_param(split_param) {}

    void visit(const IntervalNode *op) {
      if (old == op->var) {
        success = true;
        // Now time to rewrite the interval, and nest the changes
        DependencyExpr rw_start = rewrite(op->start);
        DependencyExpr rw_end = rewrite(op->end);
        DependencyExpr rw_step = rewrite(op->step);
        DependencySubset rw_child = rewrite(op->child);

        subset = Interval(
            outer, rw_start, rw_end, rw_step * (DependencyExpr(split_param)),
            Interval(inner, 0, rw_step * DependencyExpr(split_param), rw_step,
                     rw_child));

      } else {
        subset = DependencySubset(op);
      }
    }

    void visit(const DependencyVariableNode *op) {
      if (getNode(old) == op) {
        expr = outer + inner;
      } else {
        expr = DependencyExpr(op);
      }
    }

    Variable old;
    Variable outer;
    Variable inner;
    int64_t split_param;
    bool success = false;
  };

  ReplaceRewriter rw(old, outer, inner, split_param);
  auto new_s = rw.rewrite(*this);

  FERN_ASSERT(rw.success, "Split command failed, the split variable was not"
                          "found in any interval");

  return new_s;
}

DependencySubset DependencySubset::parrallelize(Variable var) {
  struct ReplaceRewriter : public DependencyRewriter {
    using DependencyRewriter::visit;
    ReplaceRewriter(Variable var) : var(var) {}

    void visit(const IntervalNode *op) {
      if (var == op->var) {
        // Can only make interval variables parrallel
        success = true;
      }

      DependencyExpr rw_start = rewrite(op->start);
      DependencyExpr rw_end = rewrite(op->end);
      DependencyExpr rw_step = rewrite(op->step);
      Variable rw_var = to<Variable>(rewrite(op->var));

      DependencySubset rw_child = rewrite(op->child);

      subset = DependencySubset(new IntervalNode(
          rw_var, rw_start, rw_end, rw_step, rw_child, op->recompute));
    }

    void visit(const DependencyVariableNode *op) {
      if (getNode(var) == op) {
        expr = Variable(op->name, op->argument, true, op->bound);
      } else {
        expr = DependencyExpr(op);
      }
    }

    Variable var;
    bool success = false;
  };

  ReplaceRewriter rw(var);
  auto new_s = rw.rewrite(*this);

  FERN_ASSERT(rw.success, "Variable was not found, or variable is not"
                          "an interval variable.");

  return new_s;
}

DependencySubset DependencySubset::bind(Variable var, int val) {
  struct ReplaceRewriter : public DependencyRewriter {
    using DependencyRewriter::visit;
    ReplaceRewriter(Variable var, int val) : var(var), val(val) {}

    void visit(const IntervalNode *op) {
      if (var == op->var) {
        // Cannot bind a variable that indexes into a interval
        success = false;
        return;
      }

      DependencyExpr rw_start = rewrite(op->start);
      DependencyExpr rw_end = rewrite(op->end);
      DependencyExpr rw_step = rewrite(op->step);
      Variable rw_var = to<Variable>(rewrite(op->var));

      DependencySubset rw_child = rewrite(op->child);

      subset = DependencySubset(new IntervalNode(
          rw_var, rw_start, rw_end, rw_step, rw_child, op->recompute));
    }

    void visit(const DependencyVariableNode *op) {
      if (getNode(var) == op) {
        success = true;
        expr = DependencyLiteral(val);
      } else {
        expr = DependencyExpr(op);
      }
    }

    Variable var;
    int val;
    int success = false;
  };

  ReplaceRewriter rw(var, val);
  auto new_s = rw.rewrite(*this);

  FERN_ASSERT(rw.success, "Variable was not found, or variable is not "
                          "free.");

  return new_s;
}

DependencySubset DependencySubset::reorder(Variable a, Variable b) {
  struct IntervalRewriter : public DependencyRewriter {
    using DependencyRewriter::visit;
    IntervalRewriter(const IntervalNode *a, const IntervalNode *b)
        : a(a), b(b) {}

    // Just replace the interval var
    void visit(const IntervalNode *op) {
      if (a == op) {
        subset = DependencySubset(new IntervalNode(
            b->var, b->start, b->end, b->step, op->child, b->recompute));
        return;
      }

      // Else proceed as normal
      DependencyExpr rw_start = rewrite(op->start);
      DependencyExpr rw_end = rewrite(op->end);
      DependencyExpr rw_step = rewrite(op->step);
      Variable rw_var = to<Variable>(rewrite(op->var));
      DependencySubset rw_child = rewrite(op->child);
      subset = DependencySubset(new IntervalNode(
          rw_var, rw_start, rw_end, rw_step, rw_child, op->recompute));
    }

    const IntervalNode *a;
    const IntervalNode *b;
  };

  struct ReorderRewriter : public DependencyRewriter {
    using DependencyRewriter::visit;
    ReorderRewriter(Variable a, Variable b) : a(a), b(b) {}

    void visit(const IntervalNode *op) {
      // Check in the current outer variable scope
      if (b == op->var) {
        // If a is an interval variable in the child of
        // b,w e can actually reorder.
        if ((op->child).is_interval_var(a)) {
          // get the interval node
          auto a_node = (op->child).get_interval_node(a);
          IntervalRewriter rw_interval(a_node, op);
          auto rw_child = rw_interval.rewrite(op->child);
          subset = DependencySubset(
              new IntervalNode(a, a_node->start, a_node->end, a_node->step,
                               rw_child, a_node->recompute));
          success = true;
          return;
        }
      }

      // Else proceed as normal
      DependencyExpr rw_start = rewrite(op->start);
      DependencyExpr rw_end = rewrite(op->end);
      DependencyExpr rw_step = rewrite(op->step);
      Variable rw_var = to<Variable>(rewrite(op->var));
      DependencySubset rw_child = rewrite(op->child);
      subset = DependencySubset(new IntervalNode(
          rw_var, rw_start, rw_end, rw_step, rw_child, op->recompute));
    }

    Variable a;
    Variable b;
    int success = false;
  };

  ReorderRewriter rw(a, b);
  auto new_s = rw.rewrite(*this);

  std::stringstream error_str;
  error_str << "Illegal reorder requested, the two variables may not exist or "
            << a << " is not a child of " << b << ".";

  FERN_ASSERT(rw.success, error_str.str());

  return new_s;
}

DependencySubset DependencySubset::recompute(Variable a) {
  struct RecomputeRewriter : public DependencyRewriter {
    using DependencyRewriter::visit;
    RecomputeRewriter(Variable a) : a(a) {}

    // Just replace the interval var
    void visit(const IntervalNode *op) {
      if (a == op->var) {
        subset = DependencySubset(new IntervalNode(op->var, op->start, op->end,
                                                   op->step, op->child, true));
        return;
      }

      // Else proceed as normal
      DependencyExpr rw_start = rewrite(op->start);
      DependencyExpr rw_end = rewrite(op->end);
      DependencyExpr rw_step = rewrite(op->step);
      Variable rw_var = to<Variable>(rewrite(op->var));
      DependencySubset rw_child = rewrite(op->child);
      subset = DependencySubset(new IntervalNode(
          rw_var, rw_start, rw_end, rw_step, rw_child, op->recompute));
    }

    Variable a;
  };

  if ((*this).is_interval_var(a)) {
    RecomputeRewriter rw(a);
    auto new_s = rw.rewrite(*this);
    return new_s;
  }

  std::stringstream error_str;
  error_str << "Variable " << a << " is not an interval variable in " << *this
            << ".";
  // we should have already returned if this error
  // did not exist.
  FERN_ASSERT(false, error_str.str());
}

bool DependencySubset::is_interval_var(Variable var) const {
  struct VarFinder : public DependencyVisitor {
    using DependencyVisitor::visit;
    VarFinder(Variable var) : var(var) {}

    void visit(const IntervalNode *op) {
      if (var == op->var) {
        success = true;
        return;
      }
    }

    Variable var;
    int success = false;
  };

  VarFinder finder(var);
  finder.visit(*this);

  return finder.success;
}

const IntervalNode *DependencySubset::get_interval_node(Variable var) const {
  struct VarFinder : public DependencyVisitor {
    using DependencyVisitor::visit;
    VarFinder(Variable var) : var(var) {}

    void visit(const IntervalNode *op) {
      if (var == op->var) {
        success = true;
        node = op;
        return;
      }
    }

    Variable var;
    const IntervalNode *node;
    int success = false;
  };

  VarFinder finder(var);
  finder.visit(*this);

  std::stringstream error_str;
  error_str << "Variable " << var << " is not an interval node in " << *this;

  FERN_ASSERT(finder.success, error_str.str());

  return finder.node;
}

} // namespace fern