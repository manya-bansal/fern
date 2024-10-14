void my_fused_impl(const Weights<float> filter, const Weights<float> input, float* bias, Weights<float> output, int64_t stride_arg74, int64_t maxpool_dim83, int64_t x_tile86, int64_t y_tile87){
  int64_t x84 = 0;
 int64_t y85 = 0;
 int64_t x79 = maxpool_dim83*x84;
 int64_t y80 = maxpool_dim83*y85;
 int64_t x_tile81 = maxpool_dim83*x_tile86;
 int64_t y_tile82 = maxpool_dim83*y_tile87;
 int64_t x75 = x79;
 int64_t y76 = y80;
 int64_t x_tile77 = x_tile81;
 int64_t y_tile78 = y_tile82;
 Weights<float> output_conv_q = weight_alloc(x75, y76, x_tile77, y_tile78);
 Weights<float> output_relu_q = weight_alloc(x79, y80, x_tile81, y_tile82);
 for(int64_t x84 = output.W_start;x84 < output.W_start + output.LW; x84+=x_tile86){
  for(int64_t y85 = output.H_start;y85 < output.H_start + output.LH; y85+=y_tile87){
      int64_t x79 = maxpool_dim83*x84;
   int64_t y80 = maxpool_dim83*y85;
   int64_t x_tile81 = maxpool_dim83*x_tile86;
   int64_t y_tile82 = maxpool_dim83*y_tile87;
   int64_t x75 = x79;
   int64_t y76 = y80;
   int64_t x_tile77 = x_tile81;
   int64_t y_tile78 = y_tile82;
   Weights<float> input_q21 = input.query_materialize(x75 * stride_arg74, y76 * stride_arg74, x_tile77 + filter.H - 1, y_tile78 + filter.W - 1);
   Weights<float> output_q = output.query_materialize(x84, y85, x_tile86, y_tile87);
      conv_no_fern_mkl(input_q21, filter, bias, stride_arg74, output_conv_q);
      relu_material(output_conv_q, output_relu_q);
      maxpool(output_relu_q, maxpool_dim83, output_q);
      output.insert_materialize(x84, y85, x_tile86, y_tile87, output_q);
      input_q21.free_weight();
}
}

  output_conv_q.free_weight();
  output_relu_q.free_weight();
}

