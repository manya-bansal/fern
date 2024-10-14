void my_fused_impl(const Weights<float> filter0, const Weights<float> input, float* bias, Weights<float> output_t, int64_t stride_arg102, int64_t stride_arg88, int64_t maxpool_dim103, int64_t x_tile106, int64_t y_tile107, int64_t maxpool_dim93){
  int64_t x104 = 0;
 int64_t y105 = 0;
 int64_t x99 = x104*maxpool_dim103;
 int64_t y98 = y105*maxpool_dim103;
 int64_t x_tile100 = maxpool_dim103*x_tile106;
 int64_t y_tile101 = y_tile107*maxpool_dim103;
 int64_t x94 = stride_arg102*x99;
 int64_t y95 = stride_arg102*y98;
 int64_t x_tile96 = -1+x_tile100+filter0.H;
 int64_t y_tile97 = -1+y_tile101+filter0.W;
 int64_t x89 = maxpool_dim93*x94;
 int64_t y90 = y95*maxpool_dim93;
 int64_t x_tile91 = maxpool_dim93*x_tile96;
 int64_t y_tile92 = y_tile97*maxpool_dim93;
 Weights<float> output_conv1_q = weight_alloc(x89, y90, x_tile91, y_tile92);
 Weights<float> output_max1_q = weight_alloc(x94, y95, x_tile96, y_tile97);
 Weights<float> output_conv2_q = weight_alloc(x99, y98, x_tile100, y_tile101);
 for(int64_t x104 = output_t.W_start;x104 < output_t.W_start + output_t.LW; x104+=x_tile106){
  for(int64_t y105 = output_t.H_start;y105 < output_t.H_start + output_t.LH; y105+=y_tile107){
      int64_t x99 = x104*maxpool_dim103;
   int64_t y98 = y105*maxpool_dim103;
   int64_t x_tile100 = maxpool_dim103*x_tile106;
   int64_t y_tile101 = y_tile107*maxpool_dim103;
   int64_t x94 = stride_arg102*x99;
   int64_t y95 = stride_arg102*y98;
   int64_t x_tile96 = -1+x_tile100+filter0.H;
   int64_t y_tile97 = -1+y_tile101+filter0.W;
   int64_t x89 = maxpool_dim93*x94;
   int64_t y90 = y95*maxpool_dim93;
   int64_t x_tile91 = maxpool_dim93*x_tile96;
   int64_t y_tile92 = y_tile97*maxpool_dim93;
   Weights<float> input_q24 = input.query_materialize(x89 * stride_arg88, y90 * stride_arg88, x_tile91 + filter0.H - 1, y_tile92 + filter0.W - 1);
   Weights<float> output_t_q = output_t.query_materialize(x104, y105, x_tile106, y_tile107);
      conv_new_mkl(input_q24, filter0, bias, stride_arg88, output_conv1_q);
      maxpool(output_conv1_q, maxpool_dim93, output_max1_q);
      conv_new_mkl(output_max1_q, filter0, bias, stride_arg102, output_conv2_q);
      maxpool(output_conv2_q, maxpool_dim103, output_t_q);
      output_t.insert_materialize(x104, y105, x_tile106, y_tile107, output_t_q);
      input_q24.free_weight();
}
}

  output_conv1_q.free_weight();
  output_max1_q.free_weight();
  output_conv2_q.free_weight();
}

