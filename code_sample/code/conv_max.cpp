void my_fused_impl(const Weights<float> filter, const Weights<float> input,
                   float *bias, Weights<float> output, int64_t stride_arg16,
                   int64_t maxpool_dim25, int64_t x_tile21, int64_t y_tile24,
                   int64_t maxpool_dim25) {
  int64_t x22 = 0;
  int64_t y23 = 0;
  int64_t x20 = maxpool_dim25 * x22;
  int64_t y19 = y23 * maxpool_dim25;
  int64_t x_tile17 = maxpool_dim25 * x_tile21;
  int64_t y_tile18 = y_tile24 * maxpool_dim25;
  int64_t x12 = x20;
  int64_t y13 = y19;
  int64_t x_tile14 = x_tile17;
  int64_t y_tile15 = y_tile18;
  Weights<float> output_conv_q = weight_alloc(x12, y13, x_tile14, y_tile15);
  Weights<float> output_relu_q = weight_alloc(x20, y19, x_tile17, y_tile18);
  for (int64_t x22 = output.W_start; x22 < output.W_start + output.LW;
       x22 += x_tile21) {
    for (int64_t y23 = output.H_start; y23 < output.H_start + output.LH;
         y23 += y_tile24) {
      int64_t x20 = maxpool_dim25 * x22;
      int64_t y19 = y23 * maxpool_dim25;
      int64_t x_tile17 = maxpool_dim25 * x_tile21;
      int64_t y_tile18 = y_tile24 * maxpool_dim25;
      int64_t x12 = x20;
      int64_t y13 = y19;
      int64_t x_tile14 = x_tile17;
      int64_t y_tile15 = y_tile18;
      Weights<float> input_q5 = input.query_materialize(
          x12 * stride_arg16, y13 * stride_arg16, x_tile14 + filter.H - 1,
          y_tile15 + filter.W - 1);
      Weights<float> output_q =
          output.query_materialize(x22, y23, x_tile21, y_tile24);
      conv_no_fern_mkl(input_q5, filter, bias, stride_arg16, output_conv_q);
      relu_material(output_conv_q, output_relu_q);
      maxpool(output_relu_q, maxpool_dim25, output_q);
      output.insert_materialize(x22, y23, x_tile21, y_tile24, output_q);
      input_q5.free_weight();
    }
  }

  output_conv_q.free_weight();
  output_relu_q.free_weight();
}
