void my_fused_impl(const Weights<float> filter, const Weights<float> input, float* bias, Weights<float> output_tan, int64_t stride_arg0, int64_t maxpool_dim9, int64_t y_tile15, int64_t x_tile17){
  int64_t x14 = 0;
 int64_t y16 = 0;
 int64_t x11 = x14;
 int64_t y12 = y16;
 int64_t x_tile10 = x_tile17;
 int64_t y_tile13 = y_tile15;
 int64_t x6 = x11*maxpool_dim9;
 int64_t y7 = y12*maxpool_dim9;
 int64_t x_tile8 = maxpool_dim9*x_tile10;
 int64_t y_tile5 = y_tile13*maxpool_dim9;
 int64_t x1 = x6;
 int64_t y2 = y7;
 int64_t x_tile3 = x_tile8;
 int64_t y_tile4 = y_tile5;
 Weights<float> output_conv_q = weight_alloc(x1, y2, x_tile3, y_tile4);
 Weights<float> output_relu_q = weight_alloc(x6, y7, x_tile8, y_tile5);
 Weights<float> output_q = weight_alloc(x11, y12, x_tile10, y_tile13);
 for(int64_t x14 = output_tan.W_start;x14 < output_tan.W_start + output_tan.LW; x14+=x_tile17){
  for(int64_t y16 = output_tan.H_start;y16 < output_tan.H_start + output_tan.LH; y16+=y_tile15){
      int64_t x11 = x14;
   int64_t y12 = y16;
   int64_t x_tile10 = x_tile17;
   int64_t y_tile13 = y_tile15;
   int64_t x6 = x11*maxpool_dim9;
   int64_t y7 = y12*maxpool_dim9;
   int64_t x_tile8 = maxpool_dim9*x_tile10;
   int64_t y_tile5 = y_tile13*maxpool_dim9;
   int64_t x1 = x6;
   int64_t y2 = y7;
   int64_t x_tile3 = x_tile8;
   int64_t y_tile4 = y_tile5;
   Weights<float> input_q1 = input.query_materialize(x1 * stride_arg0, y2 * stride_arg0, x_tile3 + filter.H - 1, y_tile4 + filter.W - 1);
   Weights<float> output_tan_q = output_tan.query_materialize(x14, y16, x_tile17, y_tile15);
      conv_no_fern_mkl(input_q1, filter, bias, stride_arg0, output_conv_q);
      relu_material(output_conv_q, output_relu_q);
      maxpool(output_relu_q, maxpool_dim9, output_q);
      tanh(output_q, output_tan_q);
      output_tan.insert_materialize(x14, y16, x_tile17, y_tile15, output_tan_q);
      input_q1.free_weight();
}
}

  output_conv_q.free_weight();
  output_relu_q.free_weight();
  output_q.free_weight();
}

