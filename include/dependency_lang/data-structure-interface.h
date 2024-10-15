#ifndef DATA_STRUCTURE_INTERFACE
#define DATA_STRUCTURE_INTERFACE

#include <iostream>
#include <ostream>
#include <set>
#include <string>
#include <vector>

// #include "dependency_lang/dep_lang.h"
#include "fern_error.h"
#include "type.h"
#include "utils/instrusive_ptr.h"
#include "utils/name.h"

namespace fern {

class DependencySubset;
class DependencyExpr;
class MetaData;
class Variable;
class ConcreteFunctionCall;
struct DependencyVariableNode;
struct DataStructureArg;

class AbstractDataStructure {
public:
  AbstractDataStructure() = default;
  virtual ~AbstractDataStructure() = default;
  // Will need to add functions to register and deregister
  // data querying and data insertion functions
  virtual std::string getTypeName() const = 0;
  virtual std::vector<std::string> getMetaData() const = 0;
  virtual std::string getDataQueryInterface() const = 0;
  virtual std::string getDataInsertInterface() const = 0;
  virtual std::string getAllocFreeInterface() const {
    return "__fern__not__defined__";
  }
  virtual std::string getQueryFreeInterface() const {
    return "__fern__not__defined__";
  }

  virtual bool insertQueried() const { return false; }
  virtual std::string getAllocData() const = 0;
  virtual bool isTranslationInvariant(int index) const {
    (void)index;
    return false;
  };

  // Q: Should I pull this out in a different class?
  virtual std::string getVarName() const { return util::uniqueName("var"); }

  // To access metadata of abstract data structures
  DependencyExpr operator[](const std::string &name) const;
  //   DataStructureArg operator()(const std::string &name) const;
};

class MyConcreteDataStructure : public AbstractDataStructure {
public:
  MyConcreteDataStructure(const AbstractDataStructure *ds,
                          const std::string &name)
      : ds(ds), name(name) {}
  // Will need to add functions to register and deregister
  // data querying and data insertion functions
  std::string getTypeName() const { return ds->getTypeName(); };
  std::vector<std::string> getMetaData() const { return ds->getMetaData(); };
  std::string getDataQueryInterface() const {
    return ds->getDataQueryInterface();
  };
  std::string getDataInsertInterface() const {
    return ds->getDataInsertInterface();
  };

  std::string getAllocFreeInterface() const {
    return ds->getAllocFreeInterface();
  }
  std::string getQueryFreeInterface() const {
    return ds->getQueryFreeInterface();
  }

  bool insertQueried() const { return ds->insertQueried(); }
  std::string getAllocData() const { return ds->getAllocData(); };
  bool isTranslationInvariant(int index) const {
    return ds->isTranslationInvariant(index);
  };
  std::string getVarName() const { return name; }

private:
  const AbstractDataStructure *ds;
  std::string name;
};

std::ostream &operator<<(std::ostream &, const AbstractDataStructure &);

class DummyDataStructure {
public:
  DummyDataStructure() = default;
  virtual ~DummyDataStructure() = default;
  // Will need to add functions to register and deregister
  // data querying and data insertion functions
  virtual std::string getTypeName() const = 0;

  // Q: Should I pull this out in a different class?
  virtual std::string getVarName() const { return util::uniqueName("var"); }

  virtual bool isConst() const { return false; }

  virtual int numRef() const { return 0; }
  virtual int numPtr() const { return 0; }

  // To access metadata of abstract data structures
  // SHOULD ADD THIS EVENTUALLY, currently no benchmarks need this
  // So delaying. Ideal Abstract and Dummy should inhert from a
  // common data structure class
  // DependencyExpr operator[](const std::string &name) const;
};

std::ostream &operator<<(std::ostream &, const DummyDataStructure &);

class DataStructurePtr {
public:
  explicit DataStructurePtr(AbstractDataStructure* ds) : ds(ds) {}

  std::ostream &print(std::ostream &os) const {
    FERN_ASSERT(ds != nullptr, "Data Structure is not defined");
    os << *(ds);
    return os;
  };

  AbstractDataStructure *dsPtr() const { return ds; };

  AbstractDataStructure * ds;
};

class DummyDataStructurePtr {
public:
  explicit DummyDataStructurePtr(DummyDataStructure *ds) : ds(ds) {}

  std::ostream &print(std::ostream &os) const {
    FERN_ASSERT(ds != nullptr, "Data Structure is not defined");
    os << *(ds);
    return os;
  };

  DummyDataStructure *dsPtr() const { return ds.get(); };

  std::shared_ptr<DummyDataStructure> ds;
};

// Different types of arguments for Functions.
enum ArgType {
  INT,
  DATASTRUCTURE,
  DUMMY_DATASTRUCTURE,
  VARIABLE,
  STRING,
  UNKNOWN
};

// TransferTypeArgs is the parent class that all internal arguments inherit
// from.
struct Args {
  Args() : argType(UNKNOWN) {}
  Args(ArgType argType) : argType(argType) {}

  virtual ~Args() = default;

  virtual std::ostream &print(std::ostream &os) const {
    os << "Print Method not defined on argument type" << std::endl;
    return os;
  };

  ArgType argType;
};

// The Argument class is a pointer to a Args object.
class Argument {
public:
  Argument(std::shared_ptr<Args> arg) : ptr_(arg) {}
  Argument(Args a) : ptr_(std::make_shared<Args>(a)) {}
  Argument() : Argument(Args()) {}

  template <typename T> const T *getNode() const {
    return static_cast<const T *>(ptr_.get());
  }
  const Args *getNode() const { return ptr_.get(); }
  ArgType getArgType() const { return getNode()->argType; };
private: 
 std::shared_ptr<Args> ptr_;
};

std::ostream &operator<<(std::ostream &, const Argument &);

// Internal argument used to a number literal argument to an external
// function.
struct LiteralArg : public Args {
  template <typename T>
  LiteralArg(Datatype datatype, T val) : Args(INT), datatype(datatype) {
    this->val = malloc(sizeof(T));
    *static_cast<T *>(this->val) = val;
  }

  ~LiteralArg() { free(val); }

  template <typename T> T getVal() const { return *static_cast<T *>(val); }

  std::ostream &print(std::ostream &os) const override {
    switch (datatype.getKind()) {
    case Datatype::Bool:
      os << this->getVal<bool>();
      break;
    case Datatype::UInt8:
      os << this->getVal<uint8_t>();
      break;
    case Datatype::UInt16:
      os << this->getVal<uint16_t>();
      break;
    case Datatype::UInt32:
      os << this->getVal<uint32_t>();
      break;
    case Datatype::UInt64:
      os << this->getVal<uint64_t>();
      break;
    case Datatype::UInt128:
      std::cerr << "Not supported" << std::endl;
      break;
    case Datatype::Int8:
      os << this->getVal<int8_t>();
      break;
    case Datatype::Int16:
      os << this->getVal<int16_t>();
      break;
    case Datatype::Int32:
      os << this->getVal<int32_t>();
      break;
    case Datatype::Int64:
      os << this->getVal<int64_t>();
      break;
    case Datatype::Int128:
      std::cerr << "Not supported" << std::endl;
      break;
    case Datatype::Float32:
      os << this->getVal<float>();
      break;
    case Datatype::Float64:
      os << this->getVal<double>();
      break;
    case Datatype::Complex64:
      os << this->getVal<std::complex<float>>();
      break;
    case Datatype::Complex128:
      os << this->getVal<std::complex<double>>();
      break;
    default:
      break;
    }
    return os;
  }

  void *val;
  Datatype datatype;
};

struct StringArg : public Args {
  StringArg(const std::string &s) : s(s) {}
  std::string getString() const { return s; }
  std::ostream &print(std::ostream &os) const override {
    os << s;
    return os;
  }
  std::string s;
};

// Internal argument used to a number literal argument to an external
// function.
struct DataStructureArg : public Args {
  DataStructureArg(DataStructurePtr ds) : Args(DATASTRUCTURE), ds(ds) {}
  DataStructureArg(DataStructurePtr ds, std::string metaData)
      : Args(DATASTRUCTURE), ds(ds), metaData(metaData), meta(true) {
    // std::cout << meta << "here" << std::endl;
    std::cout << *(ds.dsPtr()) << std::endl;
  }
  DataStructureArg(AbstractDataStructure *ds, std::string metaData, bool meta)
      : Args(DATASTRUCTURE), ds(DataStructurePtr(ds)), metaData(metaData),
        meta(meta) {}
  DataStructureArg(AbstractDataStructure *ds, std::string metaData)
      : DataStructureArg(ds, metaData, true) {}
  DataStructureArg(DataStructurePtr ds, std::string metaData, bool meta)
      : Args(DATASTRUCTURE), ds(ds), metaData(metaData), meta(meta) {}

  DataStructurePtr getDataStructure() const { return ds; }

  std::ostream &print(std::ostream &os) const override {
    ds.print(os);
    return os;
  }

  DataStructureArg operator[](std::string metaData) {
    return DataStructureArg(ds, metaData);
  }

  AbstractDataStructure *dsPtr() const { return ds.dsPtr(); }

  DataStructurePtr ds;
  std::string metaData;
  bool meta;
};

struct DummyDataStructureArg : public Args {

  DummyDataStructureArg(DummyDataStructure *ds_raw)
      : DummyDataStructureArg(DummyDataStructurePtr(ds_raw)) {}

  DummyDataStructureArg(DummyDataStructurePtr ds)
      : Args(DUMMY_DATASTRUCTURE), ds(ds) {}

  DummyDataStructurePtr getDataStructure() const { return ds; }

  std::ostream &print(std::ostream &os) const override {
    ds.print(os);
    return os;
  }

  DummyDataStructure *dsPtr() const { return ds.dsPtr(); }

  DummyDataStructurePtr ds;
  // TODO: Should eventually come back and add
  // std::string metaData;
  // bool meta;
};

struct VariableArg : public Args {
  VariableArg(const DependencyVariableNode *var) : Args(VARIABLE), var(var) {}

  const DependencyVariableNode *getVariable() const { return var; }

  std::ostream &print(std::ostream &os) const override;

  const DependencyVariableNode *var;
};

} // namespace fern

#endif