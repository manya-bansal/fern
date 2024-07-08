void my_fused_impl(const Weights<float> filter, const Weights<float> input,
                   float *bias, Weights<float> output, int64_t stride_arg58,
                   int64_t maxpool_dim63, int64_t x_tile66, int64_t y_tile67) {
  int64_t x64 = 0;
  int64_t y65 = 0;
  int64_t x59 = x64 * maxpool_dim63;
  int64_t y60 = y65 * maxpool_dim63;
  int64_t x_tile61 = x_tile66 * maxpool_dim63;
  int64_t y_tile62 = y_tile67 * maxpool_dim63;
  int64_t x54 = x59;
  int64_t y55 = y60;
  int64_t x_tile56 = x_tile61;
  int64_t y_tile57 = y_tile62;
  Weights<float> output_conv_q = weight_alloc(x54, y55, x_tile56, y_tile57);
  Weights<float> output_relu_q = weight_alloc(x59, y60, x_tile61, y_tile62);
  for (int64_t x64 = output.W_start; x64 < output.W_start + output.LW;
       x64 += x_tile66) {
    for (int64_t y65 = output.H_start; y65 < output.H_start + output.LH;
         y65 += y_tile67) {
      int64_t x59 = x64 * maxpool_dim63;
      int64_t y60 = y65 * maxpool_dim63;
      int64_t x_tile61 = x_tile66 * maxpool_dim63;
      int64_t y_tile62 = y_tile67 * maxpool_dim63;
      int64_t x54 = x59;
      int64_t y55 = y60;
      int64_t x_tile56 = x_tile61;
      int64_t y_tile57 = y_tile62;
      Weights<float> input_q15 = input.query_materialize(
          x54 * stride_arg58, y55 * stride_arg58, x_tile56 + filter.H - 1,
          y_tile57 + filter.W - 1);
      Weights<float> output_q =
          output.query_materialize(x64, y65, x_tile66, y_tile67);
      conv_no_fern_mkl(input_q15, filter, bias, stride_arg58, output_conv_q);
      relu_material(output_conv_q, output_relu_q);
      maxpool(output_relu_q, maxpool_dim63, output_q);
      output.insert_materialize(x64, y65, x_tile66, y_tile67, output_q);
      input_q15.free_weight();
    }
  }

  output_conv_q.free_weight();
  output_relu_q.free_weight();
}
