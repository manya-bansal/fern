#include <gtest/gtest.h>

#include <iostream>

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "dependency_resolver/dependency_solver.h"
#include "dependency_resolver/pipeline.h"

#include "codegen/codegen.h"

#include "examples/common-ds.h"
#include "examples/db-interface.h"
#include "examples/geospatial.h"
#include "examples/halide-examples.h"
#include "examples/image-convs.h"
#include "examples/pointwise-array.h"
#include "examples/demo.h"

#include "utils/printer.h"

#include "global_config.h"

using namespace fern;

TEST(Demo, SimpleExample) {

  examples::Array<float> d_in("d_in");
  examples::Array<float> d_out_1("d_out_1");
  examples::Array<float> d_out_2("d_out_2");
  Variable len("arg_len", true);

  examples::kernel_1_whole kernel_1_whole;
  examples::kernel_2_whole kernel_2_whole;

  Pipeline pipeline({
      kernel_1_whole(DataStructureArg(&d_in, "data"),
                DataStructureArg(&d_out_1, "data")),
      kernel_2_whole(DataStructureArg(&d_out_1, "data"),
                DataStructureArg(&d_out_2, "data")),
  });

  pipeline.constructPipeline();
  pipeline = pipeline.finalize();

  std::cout << pipeline << std::endl;

  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/demo" +
                              "/demo-1.cpp");
}