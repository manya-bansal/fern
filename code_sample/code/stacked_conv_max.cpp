void my_fused_impl(const Weights<float> filter0, const Weights<float> input, float* bias, Weights<float> output_t, int64_t stride_arg88, int64_t maxpool_dim93, int64_t stride_arg98, int64_t x_tile104, int64_t maxpool_dim103, int64_t y_tile107){
  int64_t x106 = 0;
 int64_t y105 = 0;
 int64_t x99 = maxpool_dim103*x106;
 int64_t y100 = y105*maxpool_dim103;
 int64_t x_tile101 = maxpool_dim103*x_tile104;
 int64_t y_tile102 = maxpool_dim103*y_tile107;
 int64_t x95 = stride_arg98*x99;
 int64_t y96 = stride_arg98*y100;
 int64_t x_tile94 = -1+x_tile101+filter0.H;
 int64_t y_tile97 = -1+filter0.W+y_tile102;
 int64_t x89 = maxpool_dim93*x95;
 int64_t y90 = maxpool_dim93*y96;
 int64_t x_tile91 = maxpool_dim93*x_tile94;
 int64_t y_tile92 = maxpool_dim93*y_tile97;
 Weights<float> output_conv1_q = weight_alloc(x89, y90, x_tile91, y_tile92);
 Weights<float> output_max1_q = weight_alloc(x95, y96, x_tile94, y_tile97);
 Weights<float> output_conv2_q = weight_alloc(x99, y100, x_tile101, y_tile102);
 for(int64_t x106 = output_t.W_start;x106 < output_t.W_start + output_t.LW; x106+=x_tile104){
  for(int64_t y105 = output_t.H_start;y105 < output_t.H_start + output_t.LH; y105+=y_tile107){
      int64_t x99 = maxpool_dim103*x106;
   int64_t y100 = y105*maxpool_dim103;
   int64_t x_tile101 = maxpool_dim103*x_tile104;
   int64_t y_tile102 = maxpool_dim103*y_tile107;
   int64_t x95 = stride_arg98*x99;
   int64_t y96 = stride_arg98*y100;
   int64_t x_tile94 = -1+x_tile101+filter0.H;
   int64_t y_tile97 = -1+filter0.W+y_tile102;
   int64_t x89 = maxpool_dim93*x95;
   int64_t y90 = maxpool_dim93*y96;
   int64_t x_tile91 = maxpool_dim93*x_tile94;
   int64_t y_tile92 = maxpool_dim93*y_tile97;
   Weights<float> input_q24 = input.query_materialize(x89 * stride_arg88, y90 * stride_arg88, x_tile91 + filter0.H - 1, y_tile92 + filter0.W - 1);
   Weights<float> output_t_q = output_t.query_materialize(x106, y105, x_tile104, y_tile107);
      conv_new_mkl(input_q24, filter0, bias, stride_arg88, output_conv1_q);
      maxpool(output_conv1_q, maxpool_dim93, output_max1_q);
      conv_new_mkl(output_max1_q, filter0, bias, stride_arg98, output_conv2_q);
      maxpool(output_conv2_q, maxpool_dim103, output_t_q);
      output_t.insert_materialize(x106, y105, x_tile104, y_tile107, output_t_q);
      input_q24.free_weight();
}
}

  output_conv1_q.free_weight();
  output_max1_q.free_weight();
  output_conv2_q.free_weight();
}

