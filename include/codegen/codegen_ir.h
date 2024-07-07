#ifndef CODEGEN_CG_H
#define CODEGEN_CG_H

// Impl more or less from TACO impl

#include "fern_error.h"
#include "type.h"
#include "utils/instrusive_ptr.h"

namespace fern {
namespace codegen {

class CGVisitorStrict;

enum class CGNodeType {
  Literal,
  Var,
  Neg,
  Sqrt,
  Add,
  Sub,
  Mul,
  Div,
  Mod,
  Rem,
  Min,
  Max,
  BitAnd,
  BitOr,
  Not,
  Eq,
  Neq,
  Gt,
  Lt,
  Gte,
  Lte,
  And,
  Or,
  BinOp,
  Cast,
  CustomCast,
  Call,
  IfThenElse,
  Case,
  Switch,
  Load,
  Malloc,
  Sizeof,
  Store,
  For,
  While,
  Block,
  Scope,
  Function,
  VarDecl,
  VarAssign,
  Yield,
  Allocate,
  Free,
  Comment,
  BlankLine,
  Print,
  GetProperty,
  Continue,
  Sort,
  Break,
  VoidCall,
  DeclObject,
  CustomObject,
  RawString,
  DeclFunc,
  Type,
  Assign,
  // Escape helps us insert arbitrary strings
  // into the generated code (no checks)
  EscapeExpr,
  EscapeStmt,
  MetaData,
  CilkFor,
};

/** Base class for backend CG */
struct CGNode : private util::Uncopyable {
  CGNode() {}
  virtual ~CGNode() {}
  virtual void accept(CGVisitorStrict *v) const = 0;

  /** Each CGNode subclasses carries a unique pointer we use to determine
   * its node type, because compiler RTTI sucks.
   */
  virtual CGNodeType type_info() const = 0;

  mutable long ref = 0;
  friend void acquire(const CGNode *node) { ++(node->ref); }
  friend void release(const CGNode *node) {
    if (--(node->ref) == 0) {
      delete node;
    }
  }
};

/** Base class for statements. */
struct BaseStmtNode : public CGNode {};

/** Base class for expression nodes, which have a type. */
struct BaseExprNode : public CGNode {
  Datatype type = Float();
};

/** Use the "curiously recurring template pattern" from Halide
 * to avoid duplicated code in CG nodes.  This provides the type
 * info for each class (and will handle visitor accept methods as
 * well).
 */
template <typename T> struct ExprNode : public BaseExprNode {
  virtual ~ExprNode() = default;
  void accept(CGVisitorStrict *v) const;
  virtual CGNodeType type_info() const { return T::_type_info; }
};

template <typename T> struct StmtNode : public BaseStmtNode {
  virtual ~StmtNode() = default;
  void accept(CGVisitorStrict *v) const;
  virtual CGNodeType type_info() const { return T::_type_info; }
};

/** CG nodes are passed around using opaque handles.  This class
 * handles type conversion, and will handle visitors.
 */
struct CGHandle : public util::IntrusivePtr<const CGNode> {
  CGHandle() : util::IntrusivePtr<const CGNode>() {}
  CGHandle(const CGNode *p) : util::IntrusivePtr<const CGNode>(p) {}

  /** Cast this CG node to its actual type. */
  template <typename T> const T *as() const {
    if (ptr && ptr->type_info() == T::_type_info) {
      return (const T *)ptr;
    } else {
      return nullptr;
    }
  }

  /** Dispatch to the corresponding visitor method */
  void accept(CGVisitorStrict *v) const { ptr->accept(v); }
};

/** An expression. */
class Expr : public CGHandle {
public:
  Expr() : CGHandle() {}

  Expr(bool);
  Expr(int8_t);
  Expr(int16_t);
  Expr(int32_t);
  Expr(int64_t);
  Expr(uint8_t);
  Expr(uint16_t);
  Expr(uint32_t);
  Expr(uint64_t);
  Expr(float);
  Expr(double);
  Expr(std::complex<float>);
  Expr(std::complex<double>);

  Expr(const BaseExprNode *expr) : CGHandle(expr) {}

  /** Get the type of this expression node */
  Datatype type() const { return ((const BaseExprNode *)ptr)->type; }
};

/** This is a custom comparator that allows
 * Exprs to be used in a map.  Inspired by Halide.
 */
class ExprCompare {
public:
  bool operator()(Expr a, Expr b) const { return a.ptr < b.ptr; }
};

/** A statement. */
class Stmt : public CGHandle {
public:
  Stmt() : CGHandle() {}
  Stmt(const BaseStmtNode *stmt) : CGHandle(stmt) {}
};

std::ostream &operator<<(std::ostream &os, const Stmt &);
std::ostream &operator<<(std::ostream &os, const Expr &);

/** A literal. */
struct Literal : public ExprNode<Literal> {
  void *val;

  template <typename T> static Expr make(T val, Datatype type) {
    Literal *lit = new Literal;
    lit->type = type;
    lit->val = malloc(sizeof(T));
    *static_cast<T *>(lit->val) = val;
    return lit;
  }

  template <typename T> static Expr make(T val) {
    return make(val, fern::type<T>());
  }

  /// Returns a zero literal of the given type.
  static Expr zero(Datatype datatype);

  ~Literal();

  template <typename T> T getVal() const {
    FERN_ASSERT(fern::type<T>() == type, "Casting to incorrect Type");
    return *static_cast<T *>(val);
  }

  // bool getBoolValue() const;
  // int64_t getIntValue() const;
  // uint64_t getUIntValue() const;
  // double getFloatValue() const;
  // std::complex<double> getComplexValue() const;

  static const CGNodeType _type_info = CGNodeType::Literal;
};

struct Type : public ExprNode<Type> {
  Type(const std::string &type_name) : type_name(type_name) {}
  static Expr make(const std::string &type_name) { return new Type(type_name); }
  std::string type_name;
  static const CGNodeType _type_info = CGNodeType::Type;
};

struct VarDecl : public ExprNode<VarDecl> {
  static Expr make(Expr type, const std::string &var_name,
                   bool is_const = false, int num_ref = 0, int num_ptr = 0) {
    VarDecl *var = new VarDecl;
    var->type = type;
    var->var_name = var_name;
    var->is_const = is_const;
    var->num_ref = num_ref;
    var->num_ptr = num_ptr;
    return var;
  }

  Expr type;
  std::string var_name;
  bool is_const = false;
  int num_ref = 0;
  int num_ptr = 0;

  static const CGNodeType _type_info = CGNodeType::VarDecl;
};

struct Var : public ExprNode<Var> {
  static Expr make(std::string name) {
    Var *v = new Var;
    v->name = name;
    return v;
  }
  std::string name;
  static const CGNodeType _type_info = CGNodeType::Var;
};

struct MetaData : public ExprNode<MetaData> {
  static Expr make(Expr var, std::string field) {
    MetaData *md = new MetaData;
    md->var = var;
    md->field = field;
    return md;
  }
  std::string field;
  Expr var;
  static const CGNodeType _type_info = CGNodeType::MetaData;
};

struct EscapeExpr : public ExprNode<EscapeExpr> {
  static Expr make(std::string code) {
    EscapeExpr *e = new EscapeExpr;
    e->code = code;
    return e;
  }
  std::string code;
  static const CGNodeType _type_info = CGNodeType::EscapeExpr;
};

struct Gt : ExprNode<Gt> {
  Expr a;
  Expr b;

  static Expr make(Expr a, Expr b) {
    Gt *node = new Gt;
    node->a = a;
    node->b = b;
    return node;
  }

  static const CGNodeType _type_info = CGNodeType::Gt;
};

struct Lt : ExprNode<Lt> {
  Expr a;
  Expr b;

  static Expr make(Expr a, Expr b) {
    Lt *node = new Lt;
    node->a = a;
    node->b = b;
    return node;
  }

  static const CGNodeType _type_info = CGNodeType::Lt;
};

struct Eq : ExprNode<Eq> {
  Expr a;
  Expr b;

  static Expr make(Expr a, Expr b) {
    Eq *node = new Eq;
    node->a = a;
    node->b = b;
    return node;
  }

  static const CGNodeType _type_info = CGNodeType::Eq;
};

struct Neq : ExprNode<Neq> {
  Expr a;
  Expr b;

  static Expr make(Expr a, Expr b) {
    Neq *node = new Neq;
    node->a = a;
    node->b = b;
    return node;
  }

  static const CGNodeType _type_info = CGNodeType::Neq;
};

struct Lte : ExprNode<Lte> {
  Expr a;
  Expr b;

  static Expr make(Expr a, Expr b) {
    Lte *node = new Lte;
    node->a = a;
    node->b = b;
    return node;
  }

  static const CGNodeType _type_info = CGNodeType::Lte;
};

struct Gte : ExprNode<Gte> {
  Expr a;
  Expr b;

  static Expr make(Expr a, Expr b) {
    Gte *node = new Gte;
    node->a = a;
    node->b = b;
    return node;
  }

  static const CGNodeType _type_info = CGNodeType::Gte;
};

struct Add : ExprNode<Add> {
  Expr a;
  Expr b;

  static Expr make(Expr a, Expr b) {
    Add *node = new Add;
    node->a = a;
    node->b = b;
    return node;
  }

  static const CGNodeType _type_info = CGNodeType::Add;
};

struct Sub : ExprNode<Sub> {
  Expr a;
  Expr b;

  static Expr make(Expr a, Expr b) {
    Sub *node = new Sub;
    node->a = a;
    node->b = b;
    return node;
  }

  static const CGNodeType _type_info = CGNodeType::Sub;
};

struct Mul : ExprNode<Mul> {
  Expr a;
  Expr b;

  static Expr make(Expr a, Expr b) {
    Mul *node = new Mul;
    node->a = a;
    node->b = b;
    return node;
  }

  static const CGNodeType _type_info = CGNodeType::Mul;
};

struct Div : ExprNode<Div> {
  Expr a;
  Expr b;

  static Expr make(Expr a, Expr b) {
    Div *node = new Div;
    node->a = a;
    node->b = b;
    return node;
  }

  static const CGNodeType _type_info = CGNodeType::Div;
};

struct Mod : ExprNode<Mod> {
  Expr a;
  Expr b;

  static Expr make(Expr a, Expr b) {
    Mod *node = new Mod;
    node->a = a;
    node->b = b;
    return node;
  }

  static const CGNodeType _type_info = CGNodeType::Mod;
};

struct EscapeStmt : public StmtNode<EscapeStmt> {
  static Stmt make(std::string code) {
    EscapeStmt *e = new EscapeStmt;
    e->code = code;
    return e;
  }
  std::string code;
  static const CGNodeType _type_info = CGNodeType::EscapeStmt;
};

/** A literal. */
struct Scope : public StmtNode<Scope> {
  Scope(Stmt stmt) : stmt(stmt){};
  Stmt getStmt() { return stmt; }
  static Stmt make(Stmt stmt) { return new Scope(stmt); }
  Stmt stmt;
  static const CGNodeType _type_info = CGNodeType::Scope;
};

struct DeclFunc : public StmtNode<DeclFunc> {
  Expr return_type;
  std::vector<Expr> args;
  Stmt body;
  std::string name;

  static Stmt make(std::string name, Expr return_type, std::vector<Expr> args,
                   Stmt body) {
    DeclFunc *declFunc = new DeclFunc;
    declFunc->name = name;
    declFunc->return_type = return_type;
    declFunc->args = args;
    declFunc->body = Scope::make(body);
    return declFunc;
  }

  static const CGNodeType _type_info = CGNodeType::DeclFunc;
};

struct Block : public StmtNode<Block> {
  std::vector<Stmt> stmts;
  static Stmt make(std::vector<Stmt> stmts) {
    Block *blk = new Block;
    blk->stmts = stmts;
    return blk;
  }

  template <typename... Stmts> static Stmt make(const Stmts &...stmts) {
    return make({stmts...});
  }

  static const CGNodeType _type_info = CGNodeType::Block;
};

enum AssignOp { AddEQ, SubEQ, EQ, MulEQ };

struct VarAssign : public StmtNode<VarAssign> {
  Expr lhs;
  Expr rhs;
  AssignOp op;

  static Stmt make(Expr lhs, Expr rhs, AssignOp op = EQ) {
    VarAssign *a = new VarAssign;
    a->rhs = rhs;
    a->lhs = lhs;
    a->op = op;
    return a;
  }
  static const CGNodeType _type_info = CGNodeType::VarAssign;
};

struct Call : public ExprNode<Call> {
  std::vector<Expr> arg;
  std::string name;

  static Expr make(std::string name, std::vector<Expr> arg) {
    Call *call = new Call;
    call->name = name;
    call->arg = arg;
    return call;
  }

  static const CGNodeType _type_info = CGNodeType::Call;
};

struct For : public StmtNode<For> {
  Stmt start;
  Expr cond;
  Stmt step;
  Stmt body;
  bool parallel;

  static Stmt make(Stmt start, Expr cond, Stmt step, Stmt body,
                   bool parallel = false) {
    For *f = new For;
    f->start = start;
    f->cond = cond;
    f->step = step;
    f->body = body;
    f->parallel = parallel;
    return f;
  }

  static const CGNodeType _type_info = CGNodeType::For;
};

struct BlankLine : public StmtNode<BlankLine> {
  static Stmt make() {
    BlankLine *b = new BlankLine;
    return b;
  }
  static const CGNodeType _type_info = CGNodeType::BlankLine;
};

struct VoidCall : public StmtNode<VoidCall> {
  // Should be of type call, just wrapping in stmt
  Expr func;

  static Stmt make(Expr func) {
    VoidCall *call = new VoidCall;
    call->func = func;
    return call;
  }
  static const CGNodeType _type_info = CGNodeType::VoidCall;
};

// struct CilkFor : public StmtNode<CilkFor> {
//     Stmt stmt;

//     static Stmt make(Stmt stmt) {
//         CilkFor* cFor = new CilkFor;
//         cFor->stmt = stmt;
//         return cFor;
//     }
//     static const CGNodeType _type_info = CGNodeType::CilkFor;
// };

template <typename E> inline bool isa(Expr e) {
  return e.defined() && dynamic_cast<const E *>(e.ptr) != nullptr;
}

template <typename S> inline bool isa(Stmt s) {
  return s.defined() && dynamic_cast<const S *>(s.ptr) != nullptr;
}

template <typename E> inline const E *to(Expr e) {
  FERN_ASSERT(isa<E>(e), "Improper Conversion");
  return static_cast<const E *>(e.ptr);
}

template <typename S> inline const S *to(Stmt s) {
  FERN_ASSERT(isa<S>(s), "Improper Conversion");
  return static_cast<const S *>(s.ptr);
}

} // namespace codegen
} // namespace fern

#endif