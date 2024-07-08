void my_fused_impl(const Weights<float> filter0, const Weights<float> input,
                   float *bias, Weights<float> output_t, int64_t maxpool_dim31,
                   int64_t stride_arg26, int64_t stride_arg36,
                   int64_t maxpool_dim41, int64_t maxpool_dim31,
                   int64_t stride_arg36, int64_t maxpool_dim41,
                   int64_t y_tile42, int64_t x_tile45) {
  int64_t x43 = 0;
  int64_t y44 = 0;
  int64_t x37 = maxpool_dim41 * x43;
  int64_t y38 = y44 * maxpool_dim41;
  int64_t x_tile39 = maxpool_dim41 * x_tile45;
  int64_t y_tile40 = y_tile42 * maxpool_dim41;
  int64_t x33 = x37 * stride_arg36;
  int64_t y34 = y38 * stride_arg36;
  int64_t x_tile32 = -1 + filter0.H + x_tile39;
  int64_t y_tile35 = -1 + y_tile40 + filter0.W;
  int64_t x27 = maxpool_dim31 * x33;
  int64_t y28 = y34 * maxpool_dim31;
  int64_t x_tile29 = maxpool_dim31 * x_tile32;
  int64_t y_tile30 = maxpool_dim31 * y_tile35;
  Weights<float> output_conv1_q = weight_alloc(x27, y28, x_tile29, y_tile30);
  Weights<float> output_max1_q = weight_alloc(x33, y34, x_tile32, y_tile35);
  Weights<float> output_conv2_q = weight_alloc(x37, y38, x_tile39, y_tile40);
  for (int64_t x43 = output_t.W_start; x43 < output_t.W_start + output_t.LW;
       x43 += x_tile45) {
    for (int64_t y44 = output_t.H_start; y44 < output_t.H_start + output_t.LH;
         y44 += y_tile42) {
      int64_t x37 = maxpool_dim41 * x43;
      int64_t y38 = y44 * maxpool_dim41;
      int64_t x_tile39 = maxpool_dim41 * x_tile45;
      int64_t y_tile40 = y_tile42 * maxpool_dim41;
      int64_t x33 = x37 * stride_arg36;
      int64_t y34 = y38 * stride_arg36;
      int64_t x_tile32 = -1 + filter0.H + x_tile39;
      int64_t y_tile35 = -1 + y_tile40 + filter0.W;
      int64_t x27 = maxpool_dim31 * x33;
      int64_t y28 = y34 * maxpool_dim31;
      int64_t x_tile29 = maxpool_dim31 * x_tile32;
      int64_t y_tile30 = maxpool_dim31 * y_tile35;
      Weights<float> input_q8 = input.query_materialize(
          x27 * stride_arg26, y28 * stride_arg26, x_tile29 + filter0.H - 1,
          y_tile30 + filter0.W - 1);
      Weights<float> output_t_q =
          output_t.query_materialize(x43, y44, x_tile45, y_tile42);
      conv_new_mkl(input_q8, filter0, bias, stride_arg26, output_conv1_q);
      maxpool(output_conv1_q, maxpool_dim31, output_max1_q);
      conv_new_mkl(output_max1_q, filter0, bias, stride_arg36, output_conv2_q);
      maxpool(output_conv2_q, maxpool_dim41, output_t_q);
      output_t.insert_materialize(x43, y44, x_tile45, y_tile42, output_t_q);
      input_q8.free_weight();
    }
  }

  output_conv1_q.free_weight();
  output_max1_q.free_weight();
  output_conv2_q.free_weight();
}
