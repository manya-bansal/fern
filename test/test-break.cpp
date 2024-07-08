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

TEST(Break, Test1) {

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
      addi_ispc(DataStructureArg(&b, "data"), 0.0f, getNode(len),
                DataStructureArg(&c, "data")),
  });

  pipeline.constructPipeline();
  pipeline = pipeline.breakPipeline(1);
  // pipeline = pipeline.finalize();

  std::cout << pipeline << std::endl;

  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/break" +
                              "/test1.cpp");
}

TEST(Break, Test2) {

  examples::Array<float> a("a");
  examples::Array<float> out_a("out_a");
  examples::Array<float> b("b");
  examples::Array<float> b2("b2");
  examples::Array<float> c("c");
  Variable len("arg_len", true);

  examples::addi_ispc addi_ispc;

  Pipeline pipeline({
      addi_ispc(DataStructureArg(&a, "data"), 0.0f, getNode(len),
                DataStructureArg(&out_a, "data")),
      addi_ispc(DataStructureArg(&out_a, "data"), 0.0f, getNode(len),
                DataStructureArg(&b, "data")),
      addi_ispc(DataStructureArg(&b, "data"), 0.0f, getNode(len),
                DataStructureArg(&b2, "data")),
      addi_ispc(DataStructureArg(&b2, "data"), 0.0f, getNode(len),
                DataStructureArg(&c, "data")),
  });

  pipeline.constructPipeline();
  pipeline = pipeline.breakPipeline(2);
  // pipeline = pipeline.finalize();

  std::cout << pipeline << std::endl;

  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/break" +
                              "/test2.cpp");
}

TEST(Break, Illegal) {

  examples::Array<float> a("a");
  examples::Array<float> out_a("out_a");
  examples::Array<float> b("b");
  examples::Array<float> b2("b2");
  examples::Array<float> c("c");
  Variable len("arg_len", true);

  examples::addi_ispc addi_ispc;

  Pipeline pipeline({
      addi_ispc(DataStructureArg(&a, "data"), 0.0f, getNode(len),
                DataStructureArg(&out_a, "data")),
      addi_ispc(DataStructureArg(&out_a, "data"), 0.0f, getNode(len),
                DataStructureArg(&b, "data")),
      addi_ispc(DataStructureArg(&b, "data"), 0.0f, getNode(len),
                DataStructureArg(&b2, "data")),
      addi_ispc(DataStructureArg(&b, "data"), 0.0f, getNode(len),
                DataStructureArg(&c, "data")),
  });

  pipeline.constructPipeline();
  EXPECT_THROW(pipeline = pipeline.breakPipeline(2), std::runtime_error);
  pipeline = pipeline.finalize();

  std::cout << pipeline << std::endl;
}

TEST(Break, Illegal2) {

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
      addi_ispc(DataStructureArg(&b, "data"), 0.0f, getNode(len),
                DataStructureArg(&c, "data")),
  });

  pipeline.constructPipeline();
  EXPECT_THROW(pipeline = pipeline.breakPipeline(3), std::runtime_error);
  pipeline = pipeline.finalize();

  std::cout << pipeline << std::endl;
}