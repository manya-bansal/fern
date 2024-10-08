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

#include "utils/printer.h"

#include "global_config.h"

using namespace fern;

TEST(NodeMerging, Test1) {
  // Deps should match, no change
  examples::Array<float> a("a");
  examples::Array<float> out_a("out_a");
  examples::Array<float> b("b");
  examples::Array<float> c("c");
  Variable len("arg_len", true);

  examples::addi_ispc addi_ispc;

  Pipeline pipeline({
      addi_ispc(DataStructureArg(&a, "data"), 0.0f, getNode(len),
                DataStructureArg(&out_a, "data")),
      addi_ispc(DataStructureArg(&out_a, "data"), 0.0f, getNode(len),
                DataStructureArg(&b, "data")),
      addi_ispc(DataStructureArg(&out_a, "data"), 0.0f, getNode(len),
                DataStructureArg(&c, "data")),
  });

  // Should be okay, the constraints are equal
  pipeline.constructMergedPipeline();
  pipeline = pipeline.finalize();

  std::cout << pipeline << std::endl;

  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) +
                              "/code_sample/node_merging" + "/test1.cpp");
}

TEST(NodeMerging, Test2) {
  // Should see a merging here
  examples::Array<float> a("a");
  examples::Array<float> out_a("out_a");
  examples::Array<float> b("b");
  examples::Array<float> b2("b2");
  examples::Array<float> c("c");
  Variable len("arg_len", true);

  examples::addi_ispc addi_ispc;
  examples::addi_ispc_extended_dep addi_ispc_ext;
  examples::add_ispc add_ispc;

  Pipeline pipeline({
      addi_ispc(DataStructureArg(&a, "data"), 0.0f, getNode(len),
                DataStructureArg(&out_a, "data")),
      addi_ispc(DataStructureArg(&out_a, "data"), 0.0f, getNode(len),
                DataStructureArg(&b, "data")),
      addi_ispc_ext(DataStructureArg(&out_a, "data"), 0.0f, getNode(len),
                    DataStructureArg(&c, "data")),
      add_ispc(DataStructureArg(&b, "data"), DataStructureArg(&c, "data"),
               getNode(len), DataStructureArg(&b2, "data")),
  });

  pipeline.constructMergedPipeline();
  pipeline = pipeline.finalize();

  std::cout << pipeline << std::endl;

  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) +
                              "/code_sample/node_merging" + "/test2.cpp");
}