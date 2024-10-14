#ifndef DEPENDENCY_LANG
#define DEPENDENCY_LANG

#include <cassert>
#include <set>

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang_nodes_abstract.h"
#include "utils/instrusive_ptr.h"
#include "utils/name.h"

namespace fern {

struct DependencyLiteralNode;
struct DependencyVariableNode;
struct DependencyArgumentNode;
struct AddNode;
struct SubNode;
struct MulNode;
struct DivNode;
struct ModNode;
struct MetaDataNode;
struct IntervalNode;
struct DataStructureNode;
struct SubsetNode;
struct ProducerNode;
struct ConsumerNode;
struct ComputationAnnotationNode;
struct Subset;
class AbstractDataStructure;

class DependencyExpr : public util::IntrusivePtr<const DependencyExprNode> {
public:
  DependencyExpr() : util::IntrusivePtr<const DependencyExprNode>(nullptr) {}
  explicit DependencyExpr(const DependencyExprNode *n)
      : util::IntrusivePtr<const DependencyExprNode>(n) {}

  /// Consturct an integer literal.
  DependencyExpr(char);
  DependencyExpr(std::string, bool argument = false);
  DependencyExpr(int8_t);
  DependencyExpr(int16_t);
  DependencyExpr(int32_t);
  DependencyExpr(int64_t);

  /// Consturct an unsigned integer literal.
  DependencyExpr(uint8_t);
  DependencyExpr(uint16_t);
  DependencyExpr(uint32_t);
  DependencyExpr(uint64_t);

  /// Consturct double literal.
  DependencyExpr(float);
  DependencyExpr(double);

  Datatype getDataType() const;
  std::set<const DependencyVariableNode *> getSymbols() const;

  void accept(DependencyExprVisitorStrict *) const;
  friend std::ostream &operator<<(std::ostream &, const DependencyExpr &);
};

DependencyExpr operator+(const DependencyExpr &, const DependencyExpr &);
DependencyExpr operator-(const DependencyExpr &, const DependencyExpr &);
DependencyExpr operator*(const DependencyExpr &, const DependencyExpr &);
DependencyExpr operator/(const DependencyExpr &, const DependencyExpr &);
DependencyExpr operator%(const DependencyExpr &, const DependencyExpr &);

template <typename SubType> bool isa(DependencyExpr);
template <typename SubType> SubType to(DependencyExpr);

class DependencyLiteral : public DependencyExpr {
public:
  DependencyLiteral() = default;
  explicit DependencyLiteral(const DependencyLiteralNode *);

  explicit DependencyLiteral(bool);
  explicit DependencyLiteral(char);
  explicit DependencyLiteral(int64_t);
  explicit DependencyLiteral(int);

  /// Returns the literal value.
  template <typename T> T getVal() const;

  /// Returns an untyped pointer to the literal value
  void *getValPtr();

  typedef DependencyLiteralNode Node;
};

class Variable : public DependencyExpr {
public:
  Variable() : Variable(util::uniqueName("var")){};
  explicit Variable(const DependencyVariableNode *);
  explicit Variable(const std::string &name, bool argument = false,
                    bool parallelize = false, bool bound = false);

  /// Returns the literal value.
  std::string getName() const;
  Datatype getType() const;
  bool isArgument() const;
  bool isParallel() const;
  bool isBound() const;

  void setParrallel();
  void setBound(int bound);
  int getBound();

  bool parrallel = false;
  bool bound = false;
  int num_bound = 0;

  typedef DependencyVariableNode Node;
};

// class Argument : public DependencyExpr {
//    public:
//     Argument() = default;
//     explicit Argument(const DependencyArgumentNode*);
//     explicit Argument(const std::string& name);

//     /// Returns the literal value.
//     std::string getName() const;

//     typedef DependencyArgumentNode Node;
// };

class Add : public DependencyExpr {
public:
  Add();
  Add(const AddNode *);
  Add(DependencyExpr a, DependencyExpr b);

  DependencyExpr getA() const;
  DependencyExpr getB() const;

  typedef AddNode Node;
};

class Sub : public DependencyExpr {
public:
  Sub();
  Sub(const SubNode *);
  Sub(DependencyExpr a, DependencyExpr b);

  DependencyExpr getA() const;
  DependencyExpr getB() const;

  typedef SubNode Node;
};

class Mul : public DependencyExpr {
public:
  Mul();
  Mul(const MulNode *);
  Mul(DependencyExpr a, DependencyExpr b);

  DependencyExpr getA() const;
  DependencyExpr getB() const;

  typedef MulNode Node;
};

class Div : public DependencyExpr {
public:
  Div();
  Div(const DivNode *);
  Div(DependencyExpr a, DependencyExpr b);

  DependencyExpr getA() const;
  DependencyExpr getB() const;

  typedef DivNode Node;
};

class Mod : public DependencyExpr {
public:
  Mod();
  Mod(const ModNode *);
  Mod(DependencyExpr a, DependencyExpr b);

  DependencyExpr getA() const;
  DependencyExpr getB() const;

  typedef ModNode Node;
};

class MetaData : public DependencyExpr {
public:
  MetaData(const MetaDataNode *);
  MetaData(const std::string &name, const AbstractDataStructure *ds);

  std::string getName();
  const AbstractDataStructure *getDataStructure();

  typedef MetaDataNode Node;
};

class DependencyIter : public util::IntrusivePtr<const DependencyIterNode> {
public:
  DependencyIter() : util::IntrusivePtr<const DependencyIterNode>(nullptr) {}
  explicit DependencyIter(const DependencyIterNode *n)
      : util::IntrusivePtr<const DependencyIterNode>(n) {}

  void accept(DependencyIterVisitorStrict *) const;
  friend std::ostream &operator<<(std::ostream &, const DependencyIter &);
};

template <typename SubType> bool isa(DependencyIter);
template <typename SubType> SubType to(DependencyIter);

class DependencySubset : public util::IntrusivePtr<const DependencySubsetNode> {
public:
  DependencySubset()
      : util::IntrusivePtr<const DependencySubsetNode>(nullptr) {}
  explicit DependencySubset(const DependencySubsetNode *n)
      : util::IntrusivePtr<const DependencySubsetNode>(n) {}

  void accept(DependencySubsetVisitorStrict *) const;
  friend std::ostream &operator<<(std::ostream &, const DependencySubset &);

  std::set<const DependencyVariableNode *> getVariables();

  // Scheduling command to split an interval
  DependencySubset split(Variable old, Variable outer, Variable inner,
                         int split_param);
  DependencySubset parrallelize(Variable var);
  DependencySubset bind(Variable var, int val);
  DependencySubset reorder(Variable a, Variable b);
  // The default option for our system is to reuse
  // however, users can explicitly specify when they
  // they would like intermediates to be recomputed
  // at the granularity of the the interval where the
  // recomputation takes place
  DependencySubset recompute(Variable a);

  bool is_interval_var(Variable var) const;
  const IntervalNode *get_interval_node(Variable var) const;
};

template <typename SubType> bool isa(DependencySubset);
template <typename SubType> SubType to(DependencySubset);

inline void addArg(std::vector<Argument> &argument, AbstractDataStructure *ds) {
  argument.push_back(Argument
  (std::make_shared<fern::DataStructureArg>(DataStructurePtr(ds))));
}

inline void addArg(std::vector<Argument> &argument, DummyDataStructure *ds) {
  argument.push_back(Argument(std::make_shared<DummyDataStructureArg>(ds)));
}

inline void addArg(std::vector<Argument> &argument, DataStructurePtr ds) {
  argument.push_back(Argument(std::make_shared<DataStructureArg>(ds)));
}

inline void addArg(std::vector<Argument> &argument, DataStructureArg d_arg) {
  argument.push_back(
      Argument(std::make_shared<DataStructureArg>(d_arg.ds, d_arg.metaData, d_arg.meta)));
}

inline void addArg(std::vector<Argument> &argument,
                   const DependencyVariableNode *node) {
  argument.push_back(Argument(std::make_shared<VariableArg>(node)));
}

inline void addArg(std::vector<Argument> &argument, int64_t val) {
  argument.push_back(Argument(std::make_shared<LiteralArg>(Datatype(Int64), val)));
}

inline void addArg(std::vector<Argument> &argument, float val) {
  argument.push_back(Argument(std::make_shared<LiteralArg>(Datatype(Float32), val)));
}

inline void addArg(std::vector<Argument> &argument, Argument arg) {
  argument.push_back(arg);
}

inline void addArg(std::vector<Argument> &argument, std::string str) {
  argument.push_back(Argument(std::make_shared<StringArg>(str)));
}

inline void addArguments(std::vector<Argument> &arg) {
  // do nothing
  (void)arg;
}

template <typename T, typename... Next>
inline void addArguments(std::vector<Argument> &arguments, T first,
                         Next... next) {
  addArg(arguments, first);
  addArguments(arguments, next...);
}

class ConcreteFunctionCall {
public:
  ConcreteFunctionCall() = default;
  ConcreteFunctionCall(std::string name, const std::vector<Argument> &arguments,
                       DependencySubset dataRelationship,
                       std::vector<Argument> abstractArguments);

  std::string getName() const { return name; }
  std::vector<Argument> getArguments() const { return arguments; }
  DependencySubset getDataRelationship() const { return dataRelationship; }
  std::vector<Argument> getAbstractArguments() const {
    return abstractArguments;
  }

  DependencySubset getOriginalDataRel() const {
    return dataRelationshipOriginal;
  }

  const AbstractDataStructure *getOutput() const { return output; };
  std::vector<Variable> getIntervalVars() const;
  std::set<const AbstractDataStructure *> getInputs() const { return inputs; };

private:
  std::string name;
  // Last one is the output
  std::vector<Argument> arguments;
  std::vector<Argument> abstractArguments;
  const AbstractDataStructure *output;
  std::set<const AbstractDataStructure *> inputs;
  DependencySubset dataRelationship;
  DependencySubset dataRelationshipOriginal;

  void populate_output();
  void populate_inputs();
  // need to make sure that calls to the same
  // function generate annotations with distinct
  // symbol names
  void mangle_abstract_names();
  bool sameTypeArguments(std::vector<Argument> a1, std::vector<Argument> a2);
};

class AbstractFunctionCall : public util::Manageable<AbstractFunctionCall> {
public:
  AbstractFunctionCall() = default;
  // AbstractFunctionCall(std::string name,
  //                      const std::vector<Argument> &arguments)
  //     : name(name), arguments(arguments) {}

  virtual std::string getName() const = 0;
  virtual DependencySubset getDataRelationship() const = 0;
  virtual std::vector<Argument> getArguments() = 0;

  template <typename Exprs> ConcreteFunctionCall operator()(Exprs expr) {
    std::vector<Argument> args;
    std::cout << "here" << std::endl;
    addArguments(args, expr);
    std::cout << "here 2" << std::endl;
    return ConcreteFunctionCall(getName(), args, this->getDataRelationship(),
                                this->getArguments());
  }

  template <typename FirstT, typename... Next>
  ConcreteFunctionCall operator()(FirstT first, Next... remaining) {
    std::vector<Argument> args;
    addArguments(args, first, remaining...);
    return ConcreteFunctionCall(getName(), args, this->getDataRelationship(),
                                this->getArguments());
  }

  ConcreteFunctionCall operator()() {
    std::vector<Argument> args;
    return ConcreteFunctionCall(getName(), args, this->getDataRelationship(),
                                this->getArguments());
  }
};

std::ostream &operator<<(std::ostream &, AbstractFunctionCall &);

std::ostream &operator<<(std::ostream &, const ConcreteFunctionCall &);

class DataStructure : public DependencySubset {
public:
  DataStructure(const DataStructureNode *);
  DataStructure(const std::string &name,
                const AbstractDataStructure *abstractData);
  DataStructure(const std::string &name,
                const AbstractDataStructure *abstractData,
                std::vector<DependencyExpr> annotations);

  std::string getName() const;
  const AbstractDataStructure *getAbstractDataStructure() const;
  std::vector<DependencyExpr> getAnnotations() const;

  DataStructure operator()(DependencyExpr expr) {
    std::vector<DependencyExpr> annot;
    annot.push_back(expr);
    return DataStructure(getName(), getAbstractDataStructure(), annot);
  }

  template <typename FirstT, typename... Args>
  DataStructure operator()(FirstT first, Args... remaining) {
    std::vector<DependencyExpr> annot;
    addAnnot(annot, first, remaining...);
    // Ensure that the size of the meta data fields and the annotations lines
    // up
    assert(annot.size() == getAbstractDataStructure()->getMetaData().size());
    return DataStructure(getName(), getAbstractDataStructure(), annot);
  }

  typedef DataStructureNode Node;

private:
  template <typename T, typename... Next>
  void addAnnot(std::vector<DependencyExpr> &annotations, T first,
                Next... next) {
    annotations.push_back(first);
    addAnnot(annotations, (next)...);
  }

  template <typename T>
  void addAnnot(std::vector<DependencyExpr> &annotations, T first) {
    annotations.push_back(first);
  }
};

class Interval : public DependencySubset {
public:
  Interval();
  Interval(const IntervalNode *);
  Interval(Variable var, DependencyExpr start, DependencyExpr end,
           DependencyExpr step, bool recompute = false);
  Interval(Variable var, DependencyExpr start, DependencyExpr end,
           DependencyExpr step, DependencySubset child, bool recompute = false);

  DependencyExpr getStart() const;
  DependencyExpr getEnd() const;
  DependencyExpr getStep() const;
  DependencySubset getChild() const;
  Variable getVar() const;
  bool isRecompute() const;

  Interval operator()(DependencySubset child) {
    return Interval(getVar(), getStart(), getEnd(), getStep(), child);
  }

  typedef IntervalNode Node;
};

class Producer : public DependencySubset {
public:
  Producer(const ProducerNode *);
  Producer(DependencySubset child);

  DependencySubset getChild() const;

  typedef ProducerNode Node;
};

class Consumer : public DependencySubset {
public:
  Consumer(const ConsumerNode *);
  Consumer(const std::vector<DependencySubset> &child);

  std::vector<DependencySubset> getChild() const;

  typedef ConsumerNode Node;
};

class ComputationAnnotation : public DependencySubset {
public:
  ComputationAnnotation(const ComputationAnnotationNode *);
  ComputationAnnotation(Producer producer, Consumer consumer);

  Producer getProducer() const;
  Consumer getConsumer() const;

  typedef ComputationAnnotationNode Node;
};

// class MetaData {
//    public:
//     MetaData(const std::string& name) : name(name) {}
//     friend std::ostream& operator<<(std::ostream&, const MetaData&);

//     Subset operator=(DependencyExpr);
//     Subset operator>(DependencyExpr);
//     Subset operator<(DependencyExpr);
//     Subset operator>=(DependencyExpr);
//     Subset operator<=(DependencyExpr);

//     std::string name;
// };

enum COMPARISON_OP { EQ, GEQ, LEQ, GREATER, LESS };

// class Subset : public DependencySubset {
//    public:
//     Subset();
//     Subset(const SubsetNode*);
//     Subset(MetaData meta, DependencyExpr expr, enum COMPARISON_OP op);

//     MetaData getMetaData() const;
//     DependencyExpr getDependency() const;
//     DependencyExpr getOp() const;

//     typedef SubsetNode Node;
// };

}; // namespace fern

#endif