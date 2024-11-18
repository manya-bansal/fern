#include <gtest/gtest.h>

#include <iostream>

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "dependency_resolver/dependency_solver.h"
#include "dependency_resolver/pipeline.h"

#include "codegen/codegen.h"
#include "utils/printer.h"

using namespace fern;

TEST(Print, Floats) {

    auto f1 = LiteralArg(Float32, 2.0f);
    f1.print(std::cout);
    std::cout << std::endl;
    auto f2 = LiteralArg(Float32, 2.54688798f);
    f2.print(std::cout);
    std::cout << std::endl;

}