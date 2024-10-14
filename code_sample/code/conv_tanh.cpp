void my_fused_impl(const Weights<float> filter, const Weights<float> input, float* bias, Weights<float> output_tan, int64_t x_tile158, int64_t y_tile159, int64_t maxpool_dim155, int64_t stride_arg142){
  int64_t x156 = 0;
 int64_t y157 = 0;
 int64_t x152 = x156;
 int64_t y153 = y157;
 int64_t x_tile151 = x_tile158;
 int64_t y_tile154 = y_tile159;
 int64_t x147 = maxpool_dim155*x152;
 int64_t y148 = y153*maxpool_dim155;
 int64_t x_tile149 = maxpool_dim155*x_tile151;
 int64_t y_tile150 = y_tile154*maxpool_dim155;
 int64_t x143 = x147;
 int64_t y144 = y148;
 int64_t x_tile145 = x_tile149;
 int64_t y_tile146 = y_tile150;
 Weights<float> output_conv_q = weight_alloc(x143, y144, x_tile145, y_tile146);
 Weights<float> output_relu_q = weight_alloc(x147, y148, x_tile149, y_tile150);
 Weights<float> output_q = weight_alloc(x152, y153, x_tile151, y_tile154);
 for(int64_t x156 = output_tan.W_start;x156 < output_tan.W_start + output_tan.LW; x156+=x_tile158){
  for(int64_t y157 = output_tan.H_start;y157 < output_tan.H_start + output_tan.LH; y157+=y_tile159){
      int64_t x152 = x156;
   int64_t y153 = y157;
   int64_t x_tile151 = x_tile158;
   int64_t y_tile154 = y_tile159;
   int64_t x147 = maxpool_dim155*x152;
   int64_t y148 = y153*maxpool_dim155;
   int64_t x_tile149 = maxpool_dim155*x_tile151;
   int64_t y_tile150 = y_tile154*maxpool_dim155;
   int64_t x143 = x147;
   int64_t y144 = y148;
   int64_t x_tile145 = x_tile149;
   int64_t y_tile146 = y_tile150;
   Weights<float> input_q34 = input.query_materialize(x143 * stride_arg142, y144 * stride_arg142, x_tile145 + filter.H - 1, y_tile146 + filter.W - 1);
   Weights<float> output_tan_q = output_tan.query_materialize(x156, y157, x_tile158, y_tile159);
      conv_no_fern_mkl(input_q34, filter, bias, stride_arg142, output_conv_q);
      relu_material(output_conv_q, output_relu_q);
      maxpool(output_relu_q, maxpool_dim155, output_q);
      tanh(output_q, output_tan_q);
      output_tan.insert_materialize(x156, y157, x_tile158, y_tile159, output_tan_q);
      input_q34.free_weight();
}
}

  output_conv_q.free_weight();
  output_relu_q.free_weight();
  output_q.free_weight();
}

