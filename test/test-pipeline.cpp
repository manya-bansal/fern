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

using namespace fern;

TEST(Eval, HalideBlur) {
  examples::image input("input");
  examples::image out1("out1");
  examples::image out2("out2");

  examples::blur_x blurx;
  examples::blur_y blury;

  Pipeline pipeline({
      blurx(&input, &out1),
      blury(&out1, &out2),
  });

  pipeline.constructPipeline();
  std::cout << pipeline << std::endl;
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
  std::cout << pipeline << std::endl;
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
  std::cout << pipeline << std::endl;
}
