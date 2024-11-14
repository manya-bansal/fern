void my_fused_impl(const Weights<float> filter0, const Weights<float> input, float* bias, Weights<float> output_t, int64_t stride_arg26, int64_t maxpool_dim31, int64_t stride_arg36, int64_t x_tile42, int64_t maxpool_dim41, int64_t y_tile45){
  int64_t x44 = 0;
 int64_t y43 = 0;
 int64_t x37 = maxpool_dim41*x44;
 int64_t y38 = maxpool_dim41*y43;
 int64_t x_tile40 = maxpool_dim41*x_tile42;
 int64_t y_tile39 = y_tile45*maxpool_dim41;
 int64_t x34 = stride_arg36*x37;
 int64_t y33 = stride_arg36*y38;
 int64_t x_tile35 = -1+x_tile40+filter0.H;
 int64_t y_tile32 = -1+filter0.W+y_tile39;
 int64_t x27 = x34*maxpool_dim31;
 int64_t y28 = maxpool_dim31*y33;
 int64_t x_tile29 = x_tile35*maxpool_dim31;
 int64_t y_tile30 = y_tile32*maxpool_dim31;
 Weights<float> output_conv1_q = weight_alloc(x27, y28, x_tile29, y_tile30);
 Weights<float> output_max1_q = weight_alloc(x34, y33, x_tile35, y_tile32);
 Weights<float> output_conv2_q = weight_alloc(x37, y38, x_tile40, y_tile39);
 for(int64_t x44 = output_t.W_start;x44 < output_t.W_start + output_t.LW; x44+=x_tile42){
  for(int64_t y43 = output_t.H_start;y43 < output_t.H_start + output_t.LH; y43+=y_tile45){
      int64_t x37 = maxpool_dim41*x44;
   int64_t y38 = maxpool_dim41*y43;
   int64_t x_tile40 = maxpool_dim41*x_tile42;
   int64_t y_tile39 = y_tile45*maxpool_dim41;
   int64_t x34 = stride_arg36*x37;
   int64_t y33 = stride_arg36*y38;
   int64_t x_tile35 = -1+x_tile40+filter0.H;
   int64_t y_tile32 = -1+filter0.W+y_tile39;
   int64_t x27 = x34*maxpool_dim31;
   int64_t y28 = maxpool_dim31*y33;
   int64_t x_tile29 = x_tile35*maxpool_dim31;
   int64_t y_tile30 = y_tile32*maxpool_dim31;
   Weights<float> input_q8 = input.query_materialize(x27 * stride_arg26, y28 * stride_arg26, x_tile29 + filter0.H - 1, y_tile30 + filter0.W - 1);
   Weights<float> output_t_q = output_t.query_materialize(x44, y43, x_tile42, y_tile45);
      conv_new_mkl(input_q8, filter0, bias, stride_arg26, output_conv1_q);
      maxpool(output_conv1_q, maxpool_dim31, output_max1_q);
      conv_new_mkl(output_max1_q, filter0, bias, stride_arg36, output_conv2_q);
      maxpool(output_conv2_q, maxpool_dim41, output_t_q);
      output_t.insert_materialize(x44, y43, x_tile42, y_tile45, output_t_q);
      input_q8.free_weight();
}
}

  output_conv1_q.free_weight();
  output_max1_q.free_weight();
  output_conv2_q.free_weight();
}

