void my_fused_impl(const Weights<float> filter, const Weights<float> input, float* bias, Weights<float> output, int64_t stride_arg12, int64_t x_tile23, int64_t y_tile24, int64_t maxpool_dim25){
  int64_t x21 = 0;
 int64_t y22 = 0;
 int64_t x20 = maxpool_dim25*x21;
 int64_t y18 = y22*maxpool_dim25;
 int64_t x_tile17 = maxpool_dim25*x_tile23;
 int64_t y_tile19 = y_tile24*maxpool_dim25;
 int64_t x13 = x20;
 int64_t y14 = y18;
 int64_t x_tile15 = x_tile17;
 int64_t y_tile16 = y_tile19;
 Weights<float> output_conv_q = weight_alloc(x13, y14, x_tile15, y_tile16);
 Weights<float> output_relu_q = weight_alloc(x20, y18, x_tile17, y_tile19);
 for(int64_t x21 = output.W_start;x21 < output.W_start + output.LW; x21+=x_tile23){
  for(int64_t y22 = output.H_start;y22 < output.H_start + output.LH; y22+=y_tile24){
      int64_t x20 = maxpool_dim25*x21;
   int64_t y18 = y22*maxpool_dim25;
   int64_t x_tile17 = maxpool_dim25*x_tile23;
   int64_t y_tile19 = y_tile24*maxpool_dim25;
   int64_t x13 = x20;
   int64_t y14 = y18;
   int64_t x_tile15 = x_tile17;
   int64_t y_tile16 = y_tile19;
   Weights<float> input_q5 = input.query_materialize(x13 * stride_arg12, y14 * stride_arg12, x_tile15 + filter.H - 1, y_tile16 + filter.W - 1);
   Weights<float> output_q = output.query_materialize(x21, y22, x_tile23, y_tile24);
      conv_no_fern_mkl(input_q5, filter, bias, stride_arg12, output_conv_q);
      relu_material(output_conv_q, output_relu_q);
      maxpool(output_relu_q, maxpool_dim25, output_q);
      output.insert_materialize(x21, y22, x_tile23, y_tile24, output_q);
      input_q5.free_weight();
}
}

  output_conv_q.free_weight();
  output_relu_q.free_weight();
}

