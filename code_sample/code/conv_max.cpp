void my_fused_impl(const Weights<float> filter, const Weights<float> input,
                   float *bias, Weights<float> output, int64_t stride_arg12,
                   int64_t maxpool_dim21, int64_t x_tile22,
                   int64_t maxpool_dim21, int64_t y_tile25) {
  int64_t x23 = 0;
  int64_t y24 = 0;
  int64_t x20 = x23 * maxpool_dim21;
  int64_t y19 = y24 * maxpool_dim21;
  int64_t x_tile17 = x_tile22 * maxpool_dim21;
  int64_t y_tile18 = y_tile25 * maxpool_dim21;
  int64_t x13 = x20;
  int64_t y14 = y19;
  int64_t x_tile15 = x_tile17;
  int64_t y_tile16 = y_tile18;
  Weights<float> output_conv_q = weight_alloc(x13, y14, x_tile15, y_tile16);
  Weights<float> output_relu_q = weight_alloc(x20, y19, x_tile17, y_tile18);
  for (int64_t x23 = output.W_start; x23 < output.W_start + output.LW;
       x23 += x_tile22) {
    for (int64_t y24 = output.H_start; y24 < output.H_start + output.LH;
         y24 += y_tile25) {
      int64_t x20 = x23 * maxpool_dim21;
      int64_t y19 = y24 * maxpool_dim21;
      int64_t x_tile17 = x_tile22 * maxpool_dim21;
      int64_t y_tile18 = y_tile25 * maxpool_dim21;
      int64_t x13 = x20;
      int64_t y14 = y19;
      int64_t x_tile15 = x_tile17;
      int64_t y_tile16 = y_tile18;
      Weights<float> input_q5 = input.query_materialize(
          x13 * stride_arg12, y14 * stride_arg12, x_tile15 + filter.H - 1,
          y_tile16 + filter.W - 1);
      Weights<float> output_q =
          output.query_materialize(x23, y24, x_tile22, y_tile25);
      conv_no_fern_mkl(input_q5, filter, bias, stride_arg12, output_conv_q);
      relu_material(output_conv_q, output_relu_q);
      maxpool(output_relu_q, maxpool_dim21, output_q);
      output.insert_materialize(x23, y24, x_tile22, y_tile25, output_q);
      input_q5.free_weight();
    }
  }

  output_conv_q.free_weight();
  output_relu_q.free_weight();
}
