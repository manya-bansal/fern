#include <gtest/gtest.h>

#include <iostream>

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "dependency_resolver/dependency_solver.h"
#include "dependency_resolver/pipeline.h"

#include "codegen/codegen.h"

#include "examples/common-ds.h"
#include "examples/py-pointwise-array.h"

#include "utils/printer.h"

#include "global_config.h"

using namespace fern;

TEST(PyPrint, WorstCase) {
  examples::PyArray<float> add_1("add_1");
  examples::PyArray<float> add_2("add_2");
  examples::PyArray<float> add_3("add_3");
  examples::PyArray<float> add_4("add_4");
  examples::PyArray<float> result("result");
  examples::PyArray<float> add_6("add_6");
  examples::PyArray<float> add_7("add_7");

  Variable len("len", true);

  examples::PyArray<float> a("a");
  examples::PyArray<float> b("b");
  examples::PyArray<float> c("c");

  examples::PyVadd pyVadd;

  Pipeline pipeline({
      pyVadd(&c, &c, &add_1),
      pyVadd(&a, &add_1, &add_2),
      pyVadd(&b, &add_2, &add_3),
      pyVadd(&b, &add_3, &add_4),
      pyVadd(&a, &add_4, &result),
  });

  std::map<const AbstractDataStructure *, const AbstractDataStructure *>
      resuable;

  resuable[&add_2] = &add_1;
  resuable[&add_3] = &add_1;
  resuable[&add_4] = &add_1;

  pipeline.constructPipeline();

  pipeline.register_resuable_allocs(resuable);
  pipeline = pipeline.finalize();

  util::printToFile(pipeline, std::string(SOURCE_DIR) + "/code_sample" +
                                  "/worst_case.ir");
  codegen::CodeGenerator code(pipeline);
  codegen::PyCodeGenPrint printer(code);

  util::printToFile(printer, std::string(SOURCE_DIR) + "/code_sample/py" +
                                 "/worst_case.py");
}
