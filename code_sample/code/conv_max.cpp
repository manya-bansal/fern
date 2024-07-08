void my_fused_impl(const Weights<float> filter, const Weights<float> input, float* bias, Weights<float> output, int64_t stride_arg12, int64_t maxpool_dim21, int64_t x_tile24, int64_t maxpool_dim21, int64_t y_tile25){
  int64_t x22 = 0;
 int64_t y23 = 0;
 int64_t x19 = maxpool_dim21*x22;
 int64_t y18 = y23*maxpool_dim21;
 int64_t x_tile17 = maxpool_dim21*x_tile24;
 int64_t y_tile20 = maxpool_dim21*y_tile25;
 int64_t x13 = x19;
 int64_t y14 = y18;
 int64_t x_tile15 = x_tile17;
 int64_t y_tile16 = y_tile20;
 Weights<float> output_conv_q = weight_alloc(x13, y14, x_tile15, y_tile16);
 Weights<float> output_relu_q = weight_alloc(x19, y18, x_tile17, y_tile20);
 for(int64_t x22 = output.W_start;x22 < output.W_start + output.LW; x22+=x_tile24){
  for(int64_t y23 = output.H_start;y23 < output.H_start + output.LH; y23+=y_tile25){
      int64_t x19 = maxpool_dim21*x22;
   int64_t y18 = y23*maxpool_dim21;
   int64_t x_tile17 = maxpool_dim21*x_tile24;
   int64_t y_tile20 = maxpool_dim21*y_tile25;
   int64_t x13 = x19;
   int64_t y14 = y18;
   int64_t x_tile15 = x_tile17;
   int64_t y_tile16 = y_tile20;
   Weights<float> input_q5 = input.query_materialize(x13 * stride_arg12, y14 * stride_arg12, x_tile15 + filter.H - 1, y_tile16 + filter.W - 1);
   Weights<float> output_q = output.query_materialize(x22, y23, x_tile24, y_tile25);
      conv_no_fern_mkl(input_q5, filter, bias, stride_arg12, output_conv_q);
      relu_material(output_conv_q, output_relu_q);
      maxpool(output_relu_q, maxpool_dim21, output_q);
      output.insert_materialize(x22, y23, x_tile24, y_tile25, output_q);
      input_q5.free_weight();
}
}

  output_conv_q.free_weight();
  output_relu_q.free_weight();
}

