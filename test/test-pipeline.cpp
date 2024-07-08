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

  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/code" +
                              "/halide_blur.cpp");
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
  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/code" +
                              "/conv_max.cpp");
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
  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/code" +
                              "/stacked_conv_max.cpp");
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

  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/code" +
                              "/halide_unsharp.cpp");
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
  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/code" +
                              "/granularity.cpp");
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
  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/code" +
                              "/conv_tanh.cpp");
}

TEST(Eval, DBProcessing) {

  examples::DbArray input("input");
  examples::DbArrayProcessing output("output");

  examples::SigmoidProcessing sigmoidProcessing;

  Pipeline pipeline({sigmoidProcessing(&input, &output)});

  pipeline.constructPipeline();
  pipeline = pipeline.finalize();
  // std::cout << pipeline << std::endl;

  util::printToFile(pipeline, std::string(SOURCE_DIR) + "/code_sample" +
                                  "/db_processing.ir");
  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/code" +
                              "/db_processing.cpp");
}

TEST(Eval, Haversine) {
  float lat1 = 0.70984286;
  float lon1 = -1.23892197;
  float MILES_CONST = 3959.0;

  examples::Array<float> lat2("lat2");
  examples::Array<float> lon2("lon2");
  examples::Array<float> dlat("dlat");
  examples::Array<float> dlat2("dlat2");
  examples::Array<float> dlat3("dlat3");
  examples::Array<float> dlat4("dlat4");
  examples::Array<float> dlat5("dlat5");
  examples::Array<float> dlat6("dlat6");
  examples::Array<float> dlat7("dlat7");

  examples::Array<float> dlon("dlon");
  examples::Array<float> dlon2("dlon2");
  examples::Array<float> dlon3("dlon3");
  examples::Array<float> dlon4("dlon4");

  examples::Array<float> a("a");
  examples::Array<float> a2("a2");
  examples::Array<float> a3("a3");
  examples::Array<float> a4("a4");
  examples::Array<float> a5("a5");
  examples::Array<float> a6("a6");
  examples::Array<float> a7("a7");

  Variable arg_len("arg_len", true);

  float lat1_cos = cos(lat1);
  // examples::Array<float> b("b");
  // examples::Array<float> c("c");
  // examples::Array<float> out("out");
  // examples::Array<float> final("final");

  examples::subi_ispc subi_ispc;
  examples::divi_ispc divi_ispc;
  examples::sin_ispc sin_ispc;
  examples::asin_ispc asin_ispc;
  examples::cos_ispc cos_ispc;
  examples::sqrt_ispc sqrt_ispc;
  examples::mul_ispc mul_ispc;
  examples::add_ispc add_ispc;
  examples::muli_ispc muli_ispc;
  // examples::muli_ispc muli_ispc_bound;

  examples::muli_ispc_8 muli_ispc_8;

  Pipeline pipeline({
      subi_ispc(DataStructureArg(&lat2, "data"), lat1, getNode(arg_len),
                DataStructureArg(&dlat, "data")),
      subi_ispc(DataStructureArg(&lon2, "data"), lon1, getNode(arg_len),
                DataStructureArg(&dlon, "data")),
      divi_ispc(DataStructureArg(&dlat, "data"), 2.0f, getNode(arg_len),
                DataStructureArg(&dlat2, "data")),
      sin_ispc(DataStructureArg(&dlat2, "data"), getNode(arg_len),
               DataStructureArg(&dlat3, "data")),
      sin_ispc(DataStructureArg(&dlat3, "data"), getNode(arg_len),
               DataStructureArg(&dlat4, "data")),
      mul_ispc(DataStructureArg(&dlat4, "data"),
               DataStructureArg(&dlat4, "data"), getNode(arg_len),
               DataStructureArg(&dlat5, "data")),
      cos_ispc(DataStructureArg(&dlat5, "data"), getNode(arg_len),
               DataStructureArg(&dlat6, "data")),
      muli_ispc(DataStructureArg(&dlat6, "data"), lat1_cos, getNode(arg_len),
                DataStructureArg(&dlat7, "data")),
      divi_ispc(DataStructureArg(&dlon, "data"), 2.0f, getNode(arg_len),
                DataStructureArg(&dlon2, "data")),
      sin_ispc(DataStructureArg(&dlon2, "data"), getNode(arg_len),
               DataStructureArg(&dlon3, "data")),
      mul_ispc(DataStructureArg(&dlon3, "data"),
               DataStructureArg(&dlon3, "data"), getNode(arg_len),
               DataStructureArg(&dlon4, "data")),
      mul_ispc(DataStructureArg(&a, "data"), DataStructureArg(&dlon4, "data"),
               getNode(arg_len), DataStructureArg(&a2, "data")),
      add_ispc(DataStructureArg(&a2, "data"), DataStructureArg(&dlat7, "data"),
               getNode(arg_len), DataStructureArg(&a3, "data")),
      sqrt_ispc(DataStructureArg(&a3, "data"), getNode(arg_len),
                DataStructureArg(&a4, "data")),
      asin_ispc(DataStructureArg(&a4, "data"), getNode(arg_len),
                DataStructureArg(&a5, "data")),
      muli_ispc(DataStructureArg(&a5, "data"), 2.0f, getNode(arg_len),
                DataStructureArg(&a6, "data")),
      // muli_ispc(DataStructureArg(&a6, "data"), MILES_CONST, (int64_t)8,
      //           DataStructureArg(&a7, "data")),
      muli_ispc(DataStructureArg(&a6, "data"), MILES_CONST, getNode(arg_len),
                DataStructureArg(&a, "data")),
  });

  // Currently do this by hand, adding code to programmatically construct next
  std::map<const AbstractDataStructure *, const AbstractDataStructure *>
      resuable;

  resuable[&dlat2] = &dlat;
  resuable[&dlat3] = &dlat;
  resuable[&dlat4] = &dlat;
  resuable[&dlat5] = &dlat;
  resuable[&dlat6] = &dlat;
  resuable[&dlat7] = &dlat;

  resuable[&dlon2] = &dlon;
  resuable[&dlon3] = &dlon;
  resuable[&dlon4] = &dlon;

  resuable[&a3] = &a2;
  resuable[&a4] = &a2;
  resuable[&a5] = &a2;
  resuable[&a6] = &a2;

  pipeline.constructPipeline();
  pipeline.register_resuable_allocs(resuable);
  pipeline = pipeline.finalize();

  util::printToFile(pipeline,
                    std::string(SOURCE_DIR) + "/code_sample" + "/haversine.ir");
  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/code" +
                              "/haversine.cpp");
}

TEST(Eval, WorstCase) {
  examples::Array<float> add_1("add_1");
  examples::Array<float> add_2("add_2");
  examples::Array<float> add_3("add_3");
  examples::Array<float> add_4("add_4");
  examples::Array<float> add_5("add_5");
  examples::Array<float> add_6("add_6");
  examples::Array<float> add_7("add_7");

  examples::Array<float> a("a");
  examples::Array<float> b("b");
  examples::Array<float> c("c");

  Variable len("len", true);

  examples::addi_ispc addi_ispc;
  examples::add_ispc add_ispc;

  Pipeline pipeline({
      addi_ispc(DataStructureArg(&c, "data"), 0.0f, getNode(len),
                DataStructureArg(&add_1, "data")),
      add_ispc(DataStructureArg(&a, "data"), DataStructureArg(&add_1, "data"),
               getNode(len), DataStructureArg(&add_2, "data")),
      add_ispc(DataStructureArg(&b, "data"), DataStructureArg(&add_2, "data"),
               getNode(len), DataStructureArg(&add_3, "data")),
      add_ispc(DataStructureArg(&b, "data"), DataStructureArg(&add_3, "data"),
               getNode(len), DataStructureArg(&add_4, "data")),
      add_ispc(DataStructureArg(&a, "data"), DataStructureArg(&add_4, "data"),
               getNode(len), DataStructureArg(&add_5, "data")),
      add_ispc(DataStructureArg(&c, "data"), DataStructureArg(&add_6, "data"),
               getNode(len), DataStructureArg(&add_7, "data")),
  });

  std::map<const AbstractDataStructure *, const AbstractDataStructure *>
      resuable;

  resuable[&add_2] = &add_1;
  resuable[&add_3] = &add_1;
  resuable[&add_4] = &add_1;
  resuable[&add_5] = &add_1;
  resuable[&add_6] = &add_1;

  pipeline.constructPipeline();

  pipeline.register_resuable_allocs(resuable);
  pipeline = pipeline.finalize();

  util::printToFile(pipeline, std::string(SOURCE_DIR) + "/code_sample" +
                                  "/worst_case.ir");
  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/code" +
                              "/worst_case.cpp");
}

TEST(Eval, ReuseHaversine) {
  float lat1 = 0.70984286;
  float lon1 = -1.23892197;
  float MILES_CONST = 3959.0;

  examples::Array<float> lat2("lat2");
  examples::Array<float> lon2("lon2");
  examples::Array<float> dlat("dlat");
  examples::Array<float> dlat2("dlat2");
  examples::Array<float> dlat3("dlat3");
  examples::Array<float> dlat4("dlat4");
  examples::Array<float> dlat5("dlat5");
  examples::Array<float> dlat6("dlat6");
  examples::Array<float> dlat7("dlat7");

  examples::Array<float> dlon("dlon");
  examples::Array<float> dlon2("dlon2");
  examples::Array<float> dlon3("dlon3");
  examples::Array<float> dlon4("dlon4");

  examples::Array<float> a("a");
  examples::Array<float> a2("a2");
  examples::Array<float> a3("a3");
  examples::Array<float> a4("a4");
  examples::Array<float> a5("a5");
  examples::Array<float> a6("a6");
  examples::Array<float> a7("a7");

  Variable arg_len("arg_len", true);

  float lat1_cos = cos(lat1);
  // examples::Array<float> b("b");
  // examples::Array<float> c("c");
  // examples::Array<float> out("out");
  // examples::Array<float> final("final");

  examples::subi_ispc subi_ispc;
  examples::divi_ispc divi_ispc;
  examples::sin_ispc sin_ispc;
  examples::asin_ispc asin_ispc;
  examples::cos_ispc cos_ispc;
  examples::sqrt_ispc sqrt_ispc;
  examples::mul_ispc mul_ispc;
  examples::add_ispc add_ispc;
  examples::muli_ispc muli_ispc;
  // examples::muli_ispc muli_ispc_bound;

  examples::muli_ispc_8 muli_ispc_8;

  Pipeline pipeline({
      subi_ispc(DataStructureArg(&lat2, "data"), lat1, getNode(arg_len),
                DataStructureArg(&dlat, "data")),
      subi_ispc(DataStructureArg(&lon2, "data"), lon1, getNode(arg_len),
                DataStructureArg(&dlon, "data")),
      divi_ispc(DataStructureArg(&dlat, "data"), 2.0f, getNode(arg_len),
                DataStructureArg(&dlat2, "data")),
      sin_ispc(DataStructureArg(&dlat2, "data"), getNode(arg_len),
               DataStructureArg(&dlat3, "data")),
      sin_ispc(DataStructureArg(&dlat3, "data"), getNode(arg_len),
               DataStructureArg(&dlat4, "data")),
      mul_ispc(DataStructureArg(&dlat4, "data"),
               DataStructureArg(&dlat4, "data"), getNode(arg_len),
               DataStructureArg(&dlat5, "data")),
      cos_ispc(DataStructureArg(&dlat5, "data"), getNode(arg_len),
               DataStructureArg(&dlat6, "data")),
      muli_ispc(DataStructureArg(&dlat6, "data"), lat1_cos, getNode(arg_len),
                DataStructureArg(&dlat7, "data")),
      divi_ispc(DataStructureArg(&dlon, "data"), 2.0f, getNode(arg_len),
                DataStructureArg(&dlon2, "data")),
      sin_ispc(DataStructureArg(&dlon2, "data"), getNode(arg_len),
               DataStructureArg(&dlon3, "data")),
      mul_ispc(DataStructureArg(&dlon3, "data"),
               DataStructureArg(&dlon3, "data"), getNode(arg_len),
               DataStructureArg(&dlon4, "data")),
      mul_ispc(DataStructureArg(&a, "data"), DataStructureArg(&dlon4, "data"),
               getNode(arg_len), DataStructureArg(&a2, "data")),
      add_ispc(DataStructureArg(&a2, "data"), DataStructureArg(&dlat7, "data"),
               getNode(arg_len), DataStructureArg(&a3, "data")),
      sqrt_ispc(DataStructureArg(&a3, "data"), getNode(arg_len),
                DataStructureArg(&a4, "data")),
      asin_ispc(DataStructureArg(&a4, "data"), getNode(arg_len),
                DataStructureArg(&a5, "data")),
      muli_ispc(DataStructureArg(&a5, "data"), 2.0f, getNode(arg_len),
                DataStructureArg(&a6, "data")),
      // muli_ispc(DataStructureArg(&a6, "data"), MILES_CONST, (int64_t)8,
      //           DataStructureArg(&a7, "data")),
      muli_ispc(DataStructureArg(&a6, "data"), MILES_CONST, getNode(arg_len),
                DataStructureArg(&a, "data")),
  });

  pipeline.constructPipeline();
  pipeline.runAutomaticIntermediateReuse();
  pipeline = pipeline.finalize();

  util::printToFile(pipeline, std::string(SOURCE_DIR) + "/code_sample" +
                                  "/reuse_haversine.ir");
  codegen::CodeGenerator code(pipeline);
  util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/code" +
                              "/reuse_haversine.cpp");
}