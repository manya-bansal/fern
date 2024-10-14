#ifndef GEOSPATIAL_LIB
#define GEOSPATIAL_LIB

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "examples/common-ds.h"
#include "utils/name.h"

#include <string>

namespace examples {

class optional_node : public fern::AbstractDataStructure {
public:
  optional_node(const std::string &name) : name(name) {}

  std::string getDataQueryInterface() const { return {"query_child"}; }

  std::string getDataInsertInterface() const { return {"insert"}; }

  std::string getAllocData() const { return {"unimpl"}; }

  std::string getTypeName() const { return "optional_node"; }

  std::string getVarName() const { return name; }

  std::vector<std::string> getMetaData() const { return {"idx"}; }

private:
  std::string name;
};

class BoundsType : public fern::DummyDataStructure {
public:
  BoundsType(const std::string &name) : name(name) {}

  std::string getTypeName() const { return "BoundsType"; }

  std::string getVarName() const { return name; }

  void setRef(bool r) { ref = r; }

  void setConst(bool c) { is_const = c; }

  int numRef() const { return ref; }

  bool isConst() const { return is_const; }

private:
  std::string name;
  int ref = 1;
  bool is_const = true;
};

class geom_geometry : public fern::DummyDataStructure {
public:
  geom_geometry(const std::string &name) : name(name) {}

  std::string getTypeName() const { return "geom::Geometry"; }

  std::string getVarName() const { return name; }

  void setConst(bool c) { is_const = c; }

  int setPtr(int p) { return ptr = p; }

  int numPtr() const { return ptr; }

  bool isConst() const { return is_const; }

private:
  std::string name;
  int ptr = 1;
  bool is_const = true;
};

class geom_prepgeom : public fern::DummyDataStructure {
public:
  geom_prepgeom(const std::string &name) : name(name) {}

  std::string getTypeName() const {
    return "geos::geom::prep::PreparedGeometry";
  }

  std::string getVarName() const { return name; }

  void setConst(bool c) { is_const = c; }

  int setPtr(int p) { return ptr = p; }

  int numPtr() const { return ptr; }

  bool isConst() const { return is_const; }

private:
  std::string name;
  int ptr = 1;
  bool is_const = true;
};

class Visitor : public fern::DummyDataStructure {
public:
  Visitor(const std::string &name) : name(name) {}

  std::string getTypeName() const { return "Visitor"; }

  std::string getVarName() const { return name; }

  void setConst(bool c) { is_const = c; }

  int setRef(int r) { return ref = r; }

  int numRef() const { return ref; }

  bool isConst() const { return is_const; }

private:
  std::string name;
  int ref = 2;
  bool is_const = true;
};

class func_def : public fern::DummyDataStructure {
public:
  func_def(const std::string &name) : name(name) {}

  std::string getTypeName() const { return "func_def"; }

  std::string getVarName() const { return name; }

private:
  std::string name;
};

class contains_helper : public fern::AbstractFunctionCall {
public:
  contains_helper() = default;

  std::string getName() const override { return "contains_helper"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);

    fern::Interval x_loop(x, 0, node["endChildren()"] - node["beginChildren()"],
                          1);

    fern::DataStructure node_out("out", &result);
    fern::DataStructure node_in("out", &node);

    return x_loop(fern::ComputationAnnotation(fern::Producer(node_out(x)),
                                              fern::Consumer({node_in(x)})));
  }

  std::vector<fern::Argument> getArguments() override {
    return {fern::Argument(std::make_shared<fern::DummyDataStructureArg>(&prepgeom)),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&node))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&result)))};
  }

  geom_prepgeom prepgeom{"prepgeom_f"};
  optional_node node{"node_f"};
  optional_node result{"result_f"};
};

class insert_helper : public fern::AbstractFunctionCall {
public:
  insert_helper() = default;

  std::string getName() const override { return "insert_helper"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);

    fern::Interval x_loop(x, 0, node["endChildren()"] - node["beginChildren()"],
                          1);

    fern::DataStructure node_out("out", &result);
    fern::DataStructure node_in("out", &node);

    return x_loop(fern::ComputationAnnotation(fern::Producer(node_out(x)),
                                              fern::Consumer({node_in(x)})));
  }

  std::vector<fern::Argument> getArguments() override {
    return {fern::Argument(std::make_shared<fern::DummyDataStructureArg>(&queryEnv_f)),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&node))),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&result)))};
  }

  BoundsType queryEnv_f{"queryEnv_f"};
  optional_node node{"node_f"};
  optional_node result{"result_f"};
};

class recurse_helper : public fern::AbstractFunctionCall {
public:
  recurse_helper() = default;
  std::string getName() const override { return "recurse_helper"; }

  fern::DependencySubset getDataRelationship() const override {

    return fern::DependencySubset();
  }

  std::vector<fern::Argument> getArguments() override {
    return {fern::Argument(std::make_shared<fern::DummyDataStructureArg>(&queryEnv_f)),
            fern::Argument(std::make_shared<fern::DummyDataStructureArg>(&func)),
            fern::Argument(std::make_shared<fern::DummyDataStructureArg>(&visitor)),
            fern::Argument(std::make_shared<fern::DummyDataStructureArg>(&prepgeom)),
            fern::Argument(std::make_shared<fern::DummyDataStructureArg>(&g)),
            fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&node)))};
  }

  BoundsType queryEnv_f{"queryEnv_f"};
  optional_node node{"node_f"};
  geom_geometry g{"g_f"};
  geom_prepgeom prepgeom{"prepgeom_f"};
  Visitor visitor{"visitor_f"};
  func_def func{"func_f"};
};

} // namespace examples
#endif