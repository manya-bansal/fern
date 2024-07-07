#include <gtest/gtest.h>

#include <iostream>

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "dependency_resolver/dependency_solver.h"
#include "dependency_resolver/pipeline.h"

#include "examples/arm-apple-interface.h"
#include "examples/common-ds.h"
#include "examples/geospatial.h"
#include "examples/halide-examples.h"
#include "examples/image-convs.h"
#include "examples/pointwise-array.h"

#include "utils/printer.h"

#include "global_config.h"

using namespace fern;

TEST(Eval, HalideBlur) {
  examples::image input("input");
  examples::image out1("out1");
  examples::image out2("out2");

  examples::blur_x blurx;
  examples::blur_y blury;

  auto blur_x_concrete = blurx(&input, &out1);

  Pipeline pipeline({
      blur_x_concrete,
      blury(&out1, &out2),
  });

  // Step one is to always construct a pipeline
  pipeline.constructPipeline();

  // Next, we can begin scheduling
  // 1: Reorder the col and row loops
  pipeline = pipeline.reorder(0, 1);
  // 1: Split rows by new var
  Variable outer("o");
  Variable inner("i");
  Variable outer_step("o_s");
  Variable inner_step("i_s");
  pipeline = pipeline.split(1, outer, inner, outer_step, inner_step);

  // 1: Parrallelize outer loop
  pipeline = pipeline.parrallelize(1);
  FERN_ASSERT_NO_MSG(pipeline.outer_loops[1].var.parrallel == true);
  // Bind the value of the inner step to 1
  // compute one row at a time
  pipeline = pipeline.bind(inner_step, 1);
  FERN_ASSERT_NO_MSG(pipeline.bounded_vars.count(inner_step) > 0);
  // Indicate that we would like to compute the reuse of a particular
  // data-structure
  auto all_interval_vars = blur_x_concrete.getIntervalVars();
  pipeline = pipeline.reuse(&out1, all_interval_vars[0]);
  // To indicate the end of scheduling we call the finalize function.
  // Indicate the end of scheduling code and the beginning of opt
  // passes
  pipeline = pipeline.finalize();
  std::cout << "*********" << std::endl;
  std::cout << pipeline << std::endl;

  util::printToFile(pipeline, std::string(SOURCE_DIR) + "/code_sample" +
                                  "/halide_blur.ir");
}

TEST(Eval, ConvMax) {
  examples::Weights input("input");
  examples::Weights filter("filter");
  examples::FloatPtr bias("bias");
  examples::Weights output_conv("output_conv");
  examples::Weights output_relu("output_relu");
  examples::Weights output("output");

  examples::Convolution_NoFuse convolution_mkl;
  examples::Relu_Material relu_material;
  examples::Maxpool maxpool;

  Variable stride_arg("stride_arg", true);
  Variable maxpool_dim("maxpool_dim", true);

  Pipeline pipeline({
      convolution_mkl(&input, &filter, &bias, getNode(stride_arg),
                      &output_conv),
      relu_material(&output_conv, &output_relu),
      maxpool(&output_relu, getNode(maxpool_dim), &output),
  });

  pipeline.constructPipeline();
  pipeline = pipeline.finalize();
  // std::cout << pipeline << std::endl;

  util::printToFile(pipeline,
                    std::string(SOURCE_DIR) + "/code_sample" + "/conv_max.ir");
}

TEST(Eval, StackedFusedConvMax) {
  examples::Weights input("input");
  examples::Weights filter("filter0");
  examples::FloatPtr bias("bias");
  examples::Weights output_conv1("output_conv1");
  examples::Weights output_conv2("output_conv2");
  examples::Weights output_max1("output_max1");
  examples::Weights output("output_t");

  examples::Convolution_Mkl_Fused convolution_mkl_fused;
  examples::Maxpool maxpool;

  Variable stride_arg("stride_arg", true);
  Variable maxpool_dim("maxpool_dim", true);

  Pipeline pipeline({
      convolution_mkl_fused(&input, &filter, &bias, getNode(stride_arg),
                            &output_conv1),
      maxpool(&output_conv1, getNode(maxpool_dim), &output_max1),
      convolution_mkl_fused(&output_max1, &filter, &bias, getNode(stride_arg),
                            &output_conv2),
      maxpool(&output_conv2, getNode(maxpool_dim), &output),
  });

  pipeline.constructPipeline();
  pipeline = pipeline.finalize();
  // std::cout << pipeline << std::endl;

  util::printToFile(pipeline, std::string(SOURCE_DIR) + "/code_sample" +
                                  "/stacked_conv_max.ir");
}

TEST(Eval, HalideUnsharp) {
  examples::image input("input");
  examples::image gray_out("gray_out");
  examples::image blur_y_temp("blur_y_temp");
  examples::image blur_x_temp("blur_x_temp");
  examples::image sharpen_temp("sharpen_temp");
  examples::image ratio_temp("ratio_temp");
  examples::image final_out("final_out");

  examples::gray gray;
  examples::blur_x_unsharp blurx;
  examples::blur_y_unsharp blury;
  examples::sharpen sharpen;
  examples::ratio ratio;
  examples::output_assign output_assign;

  auto gray_concrete = gray(&input, &gray_out);

  Pipeline pipeline({gray_concrete, blury(&gray_out, &blur_y_temp),
                     blurx(&blur_y_temp, &blur_x_temp),
                     sharpen(&gray_out, &blur_x_temp, &sharpen_temp),
                     ratio(&gray_out, &sharpen_temp, &ratio_temp),
                     output_assign(&input, &ratio_temp, &final_out)});

  // Step one is to always construct a pipeline
  pipeline.constructPipeline();

  // Next, we can begin scheduling
  // 1: Reorder the col and row loops
  pipeline = pipeline.reorder(0, 1);
  // 1: Split rows by new var
  Variable outer("o");
  Variable inner("i");
  Variable outer_step("o_s");
  Variable inner_step("i_s");
  pipeline = pipeline.split(1, outer, inner, outer_step, inner_step);

  // 1: Parrallelize outer loop
  pipeline = pipeline.parrallelize(1);
  FERN_ASSERT_NO_MSG(pipeline.outer_loops[1].var.parrallel == true);
  // Bind the value of the inner step to 1
  // compute one row at a time
  pipeline = pipeline.bind(inner_step, 1);
  FERN_ASSERT_NO_MSG(pipeline.bounded_vars.count(inner_step) > 0);
  // Indicate that we would like to compute the reuse of a particular
  // data-structure
  auto all_interval_vars = gray_concrete.getIntervalVars();
  pipeline = pipeline.reuse(&gray_out, all_interval_vars[0]);
  // To indicate the end of scheduling we call the finalize function.
  // Indicate the end of scheduling code and the beginning of opt
  // passes
  pipeline = pipeline.finalize();
  // std::cout << pipeline << std::endl;
  util::printToFile(pipeline, std::string(SOURCE_DIR) + "/code_sample" +
                                  "/halide_unsharp.ir");
}

TEST(Eval, Granularity) {
  examples::Array<float> a("a");
  examples::Array<float> b("b");
  examples::Array<float> c("c");

  examples::Array<float> r1("r1");
  examples::Array<float> r2("r2");
  examples::Array<float> r3("r3");

  examples::vadd vadd;
  examples::vadd vmul;
  examples::vadd vsub;

  Pipeline pipeline({
      vadd(&a, &b, &r1),
      vmul(&r1, &c, &r2),
      vsub(&r1, &r2, &r3),
  });

  pipeline.constructPipeline();
  pipeline = pipeline.finalize();

  util::printToFile(pipeline, std::string(SOURCE_DIR) + "/code_sample" +
                                  "/granularity.ir");
}

TEST(Eval, FusedConvMaxTanh) {

  examples::Weights input("input");
  examples::Weights filter("filter");
  examples::FloatPtr bias("bias");
  examples::Weights output_conv("output_conv");
  examples::Weights output_relu("output_relu");
  examples::Weights output("output");
  examples::Weights output_tan("output_tan");

  examples::Convolution_NoFuse convolution_mkl;
  examples::Relu_Material relu_material;
  examples::Maxpool maxpool;
  examples::Tanh tanh;

  Variable stride_arg("stride_arg", true);
  Variable maxpool_dim("maxpool_dim", true);

  Pipeline pipeline({
      convolution_mkl(&input, &filter, &bias, getNode(stride_arg),
                      &output_conv),
      relu_material(&output_conv, &output_relu),
      maxpool(&output_relu, getNode(maxpool_dim), &output),
      tanh(&output, &output_tan),
  });

  pipeline.constructPipeline();
  pipeline = pipeline.finalize();
  // std::cout << pipeline << std::endl;

  util::printToFile(pipeline,
                    std::string(SOURCE_DIR) + "/code_sample" + "/conv_tanh.ir");
}