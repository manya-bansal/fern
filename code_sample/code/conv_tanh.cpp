void my_fused_impl(const Weights<float> filter, const Weights<float> input, float* bias, Weights<float> output_tan, int64_t stride_arg142, int64_t maxpool_dim151, int64_t x_tile157, int64_t y_tile159){
  int64_t x158 = 0;
 int64_t y156 = 0;
 int64_t x153 = x158;
 int64_t y154 = y156;
 int64_t x_tile152 = x_tile157;
 int64_t y_tile155 = y_tile159;
 int64_t x150 = x153*maxpool_dim151;
 int64_t y149 = maxpool_dim151*y154;
 int64_t x_tile147 = maxpool_dim151*x_tile152;
 int64_t y_tile148 = maxpool_dim151*y_tile155;
 int64_t x143 = x150;
 int64_t y144 = y149;
 int64_t x_tile145 = x_tile147;
 int64_t y_tile146 = y_tile148;
 Weights<float> output_conv_q = weight_alloc(x143, y144, x_tile145, y_tile146);
 Weights<float> output_relu_q = weight_alloc(x150, y149, x_tile147, y_tile148);
 Weights<float> output_q = weight_alloc(x153, y154, x_tile152, y_tile155);
 for(int64_t x158 = output_tan.W_start;x158 < output_tan.W_start + output_tan.LW; x158+=x_tile157){
  for(int64_t y156 = output_tan.H_start;y156 < output_tan.H_start + output_tan.LH; y156+=y_tile159){
      int64_t x153 = x158;
   int64_t y154 = y156;
   int64_t x_tile152 = x_tile157;
   int64_t y_tile155 = y_tile159;
   int64_t x150 = x153*maxpool_dim151;
   int64_t y149 = maxpool_dim151*y154;
   int64_t x_tile147 = maxpool_dim151*x_tile152;
   int64_t y_tile148 = maxpool_dim151*y_tile155;
   int64_t x143 = x150;
   int64_t y144 = y149;
   int64_t x_tile145 = x_tile147;
   int64_t y_tile146 = y_tile148;
   Weights<float> input_q34 = input.query_materialize(x143 * stride_arg142, y144 * stride_arg142, x_tile145 + filter.H - 1, y_tile146 + filter.W - 1);
   Weights<float> output_tan_q = output_tan.query_materialize(x158, y156, x_tile157, y_tile159);
      conv_no_fern_mkl(input_q34, filter, bias, stride_arg142, output_conv_q);
      relu_material(output_conv_q, output_relu_q);
      maxpool(output_relu_q, maxpool_dim151, output_q);
      tanh(output_q, output_tan_q);
      output_tan.insert_materialize(x158, y156, x_tile157, y_tile159, output_tan_q);
      input_q34.free_weight();
}
}

  output_conv_q.free_weight();
  output_relu_q.free_weight();
  output_q.free_weight();
}

