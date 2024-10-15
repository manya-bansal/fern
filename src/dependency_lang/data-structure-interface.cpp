#include "dependency_lang/data-structure-interface.h"

#include <iostream>
#include <map>

#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "dependency_lang/dep_lang_rewriter.h"
#include "fern_error.h"
#include "utils/printer.h"

namespace fern {

DependencyExpr
AbstractDataStructure::operator[](const std::string &name) const {
  return DependencyExpr(MetaData(name, this));
}

std::ostream &operator<<(std::ostream &os, const AbstractDataStructure &ds) {
  os << ds.getTypeName() << " " << ds.getVarName() << " {";
  os << std::endl;
  auto metaDataFields = ds.getMetaData();
  for (int i = 0; i < metaDataFields.size(); i++) {
    os << metaDataFields[i] << ", ";
  }
  os << "}";
  return os;
}

std::ostream &operator<<(std::ostream &os, const DummyDataStructure &ds) {
  os << ds.getTypeName() << " " << ds.getVarName();
  return os;
}

std::ostream &operator<<(std::ostream &os, const Argument &argument) {
  return argument.getNode()->print(os);
}

ConcreteFunctionCall::ConcreteFunctionCall(
    std::string name, const std::vector<Argument> &arguments,
    DependencySubset dataRelationship, std::vector<Argument> abstractArguments)
    : name(name), arguments(arguments), dataRelationship(dataRelationship),
      abstractArguments(abstractArguments),
      dataRelationshipOriginal(dataRelationship) {
  FERN_ASSERT(sameTypeArguments(arguments, abstractArguments),
              "Types of arguments do not match");
  // need to mangle names because one abstract function
  // can be the source of multiple concrete function calls.

  populate_inputs();

  populate_output();

  mangle_abstract_names();
}

std::vector<Variable> ConcreteFunctionCall::getIntervalVars() const {
  std::vector<Variable> all_vars;

  match(dataRelationship, std::function<void(const IntervalNode *, Matcher *)>(
                              [&](const IntervalNode *op, Matcher *ctx) {
                                all_vars.push_back(op->var);
                                ctx->match(op->child);
                              }));

  return all_vars;
}

bool ConcreteFunctionCall::sameTypeArguments(std::vector<Argument> a1,
                                             std::vector<Argument> a2) {

  if (a1.size() != a2.size()) {
    std::cout << a1.size() << std::endl;
    util::printIterable(a1);
    std::cout << a2.size() << std::endl;
    util::printIterable(a2);
    return false;
  }

  for (int i = 0; i < a1.size(); i++) {
    if (a1[i].getArgType() != a2[i].getArgType()) {
      std::cout << i << std::endl;
      return false;
    }
  }

  return true;
}

void ConcreteFunctionCall::populate_output() {
  // Should at least have one
  FERN_ASSERT(arguments.size() > 0,
              "Should at least have one argument (output)");
  auto output_func = arguments[arguments.size() - 1];

  FERN_ASSERT(output_func.getArgType() == DATASTRUCTURE,
              "The output type should be a datastructure argument");

  output = output_func.getNode<DataStructureArg>()->dsPtr();
}

void ConcreteFunctionCall::populate_inputs() {
  // Should at least have one
  for (int i = 0; i < arguments.size() - 1; i++) {
    // If it's not a data structure, we can skip for now
    if (arguments[i].getArgType() != DATASTRUCTURE) {
      continue;
    }
    inputs.insert(arguments[i].getNode<DataStructureArg>()->dsPtr());
  }
}

void ConcreteFunctionCall::mangle_abstract_names() {
  std::map<const DependencyVariableNode *, const DependencyVariableNode *>
      mangled_names;
  auto all_vars = dataRelationship.getVariables();

  for (auto var : all_vars) {
    mangled_names[var] =
        (new DependencyVariableNode(util::uniqueName(var->name), var->argument,
                                    var->parallelize, var->bound));
  }

  // Given the mangled names, rewrite the data-relationships
  dataRelationship = ChangeVariableNames(dataRelationship, mangled_names);

  // And, also need to change the arguments if the contain any of the variables
  for (int i = 0; i < abstractArguments.size(); i++) {
    auto arg = abstractArguments[i];
    if (arg.getArgType() == VARIABLE) {
      auto var = arg.getNode<VariableArg>()->getVariable();
      // if (!(var->argument)) {
      std::cout << this->getDataRelationship() << std::endl;
      std::cout << var->name << std::endl;
      util::printIterable(getAbstractArguments());
      FERN_ASSERT(mangled_names.find(var) != mangled_names.end(),
                  "Function is trying to use an argument that comes out from "
                  "nowhere??");
      arguments[i] =  Argument(std::make_shared<VariableArg>(mangled_names[var]));
      // abstractArguments[i] = new VariableArg(mangled_names[var]);
      // }
    }
  }

  // change the meta nodes to match the arguments passed in
  struct ReplaceRewriter : public DependencyRewriter {
    using DependencyRewriter::visit;
    ReplaceRewriter(const AbstractDataStructure *d_abstract,
                    const AbstractDataStructure *d_concrete)
        : d_abstract(d_abstract), d_concrete(d_concrete) {}

    void visit(const MetaDataNode *op) {
      if (op->ds == d_abstract) {
        expr = DependencyExpr(new MetaDataNode(op->name, d_concrete));
      } else {
        expr = DependencyExpr(op);
      }
    }

    const AbstractDataStructure *d_abstract;
    const AbstractDataStructure *d_concrete;
  };

  for (int j = 0; j < arguments.size(); j++) {
    auto concrete_arg = arguments[j];
    auto abstract_arg = abstractArguments[j];
    if (concrete_arg.getArgType() == DATASTRUCTURE) {
      auto abstract_ds = abstract_arg.getNode<DataStructureArg>()->dsPtr();
      auto concrete_ds = concrete_arg.getNode<DataStructureArg>()->dsPtr();
      ReplaceRewriter rw(abstract_ds, concrete_ds);
      dataRelationship = rw.rewrite(dataRelationship);
    }
  }
}

// std::vector<const AbstractDataStructure*> ConcreteFunctionCall::getInputs()
// const {
//     std::vector<const AbstractDataStructure*> inputs;
// }

// Argument ConcreteFunctionCall::getInput() {
//     std::
//     // Should at least have one
//     FERN_ASSERT(arguments.size() > 0,
//                 "Should at least have one argument (output)");
//     return arguments[arguments.size() - 1];
// };

std::ostream &operator<<(std::ostream &os, AbstractFunctionCall &func) {
  os << func.getName() << "(";

  auto arguments = func.getArguments();

  if (arguments.size() == 0) {
    os << "()";
    return os;
  }

  for (int i = 0; i < arguments.size() - 1; i++) {
    os << arguments[i] << ", ";
  }

  os << arguments[arguments.size() - 1] << " (output) )";
  return os;
}

std::ostream &operator<<(std::ostream &os, const ConcreteFunctionCall &func) {
  os << func.getName() << "(";
  auto arguments = func.getArguments();
  std::cout << std::endl;
  if (arguments.size() == 0) {
    os << "()";
    return os;
  }
    std::cout << "arguments.size() " << arguments.size() << std::endl;
  for (int i = 0; i < arguments.size() - 1; i++) {
    os << arguments[i] << ", ";
  }

  os << arguments[arguments.size() - 1] << " (output) )";
  return os;
}

std::ostream &VariableArg::print(std::ostream &os) const {
  os << Variable(var);
  return os;
}

} // namespace fern
