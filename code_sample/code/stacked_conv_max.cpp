void my_fused_impl(const Weights<float> filter0, const Weights<float> input, float* bias, Weights<float> output_t, int64_t maxpool_dim77, int64_t stride_arg82, int64_t x_tile84, int64_t y_tile86, int64_t maxpool_dim87, int64_t stride_arg69){
  int64_t x85 = 0;
 int64_t y83 = 0;
 int64_t x78 = maxpool_dim87*x85;
 int64_t y79 = y83*maxpool_dim87;
 int64_t x_tile80 = maxpool_dim87*x_tile84;
 int64_t y_tile81 = maxpool_dim87*y_tile86;
 int64_t x73 = x78*stride_arg82;
 int64_t y75 = y79*stride_arg82;
 int64_t x_tile74 = -1+x_tile80+filter0.H;
 int64_t y_tile76 = -1+y_tile81+filter0.W;
 int64_t x70 = maxpool_dim77*x73;
 int64_t y71 = maxpool_dim77*y75;
 int64_t x_tile68 = maxpool_dim77*x_tile74;
 int64_t y_tile72 = maxpool_dim77*y_tile76;
 Weights<float> output_conv1_q = weight_alloc(x70, y71, x_tile68, y_tile72);
 Weights<float> output_max1_q = weight_alloc(x73, y75, x_tile74, y_tile76);
 Weights<float> output_conv2_q = weight_alloc(x78, y79, x_tile80, y_tile81);
 for(int64_t x85 = output_t.W_start;x85 < output_t.W_start + output_t.LW; x85+=x_tile84){
  for(int64_t y83 = output_t.H_start;y83 < output_t.H_start + output_t.LH; y83+=y_tile86){
      int64_t x78 = maxpool_dim87*x85;
   int64_t y79 = y83*maxpool_dim87;
   int64_t x_tile80 = maxpool_dim87*x_tile84;
   int64_t y_tile81 = maxpool_dim87*y_tile86;
   int64_t x73 = x78*stride_arg82;
   int64_t y75 = y79*stride_arg82;
   int64_t x_tile74 = -1+x_tile80+filter0.H;
   int64_t y_tile76 = -1+y_tile81+filter0.W;
   int64_t x70 = maxpool_dim77*x73;
   int64_t y71 = maxpool_dim77*y75;
   int64_t x_tile68 = maxpool_dim77*x_tile74;
   int64_t y_tile72 = maxpool_dim77*y_tile76;
   Weights<float> input_q18 = input.query_materialize(x70 * stride_arg69, y71 * stride_arg69, x_tile68 + filter0.H - 1, y_tile72 + filter0.W - 1);
   Weights<float> output_t_q = output_t.query_materialize(x85, y83, x_tile84, y_tile86);
      conv_new_mkl(input_q18, filter0, bias, stride_arg69, output_conv1_q);
      maxpool(output_conv1_q, maxpool_dim77, output_max1_q);
      conv_new_mkl(output_max1_q, filter0, bias, stride_arg82, output_conv2_q);
      maxpool(output_conv2_q, maxpool_dim87, output_t_q);
      output_t.insert_materialize(x85, y83, x_tile84, y_tile86, output_t_q);
      input_q18.free_weight();
}
}

  output_conv1_q.free_weight();
  output_max1_q.free_weight();
  output_conv2_q.free_weight();
}

