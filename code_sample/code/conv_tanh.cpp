void my_fused_impl(const Weights<float> filter, const Weights<float> input, float* bias, Weights<float> output_tan, int64_t x_tile137, int64_t y_tile139, int64_t stride_arg122, int64_t maxpool_dim131){
  int64_t x136 = 0;
 int64_t y138 = 0;
 int64_t x135 = x136;
 int64_t y132 = y138;
 int64_t x_tile133 = x_tile137;
 int64_t y_tile134 = y_tile139;
 int64_t x128 = x135*maxpool_dim131;
 int64_t y127 = y132*maxpool_dim131;
 int64_t x_tile129 = maxpool_dim131*x_tile133;
 int64_t y_tile130 = y_tile134*maxpool_dim131;
 int64_t x123 = x128;
 int64_t y124 = y127;
 int64_t x_tile125 = x_tile129;
 int64_t y_tile126 = y_tile130;
 Weights<float> output_conv_q = weight_alloc(x123, y124, x_tile125, y_tile126);
 Weights<float> output_relu_q = weight_alloc(x128, y127, x_tile129, y_tile130);
 Weights<float> output_q = weight_alloc(x135, y132, x_tile133, y_tile134);
 for(int64_t x136 = output_tan.W_start;x136 < output_tan.W_start + output_tan.LW; x136+=x_tile137){
  for(int64_t y138 = output_tan.H_start;y138 < output_tan.H_start + output_tan.LH; y138+=y_tile139){
      int64_t x135 = x136;
   int64_t y132 = y138;
   int64_t x_tile133 = x_tile137;
   int64_t y_tile134 = y_tile139;
   int64_t x128 = x135*maxpool_dim131;
   int64_t y127 = y132*maxpool_dim131;
   int64_t x_tile129 = maxpool_dim131*x_tile133;
   int64_t y_tile130 = y_tile134*maxpool_dim131;
   int64_t x123 = x128;
   int64_t y124 = y127;
   int64_t x_tile125 = x_tile129;
   int64_t y_tile126 = y_tile130;
   Weights<float> input_q28 = input.query_materialize(x123 * stride_arg122, y124 * stride_arg122, x_tile125 + filter.H - 1, y_tile126 + filter.W - 1);
   Weights<float> output_tan_q = output_tan.query_materialize(x136, y138, x_tile137, y_tile139);
      conv_no_fern_mkl(input_q28, filter, bias, stride_arg122, output_conv_q);
      relu_material(output_conv_q, output_relu_q);
      maxpool(output_relu_q, maxpool_dim131, output_q);
      tanh(output_q, output_tan_q);
      output_tan.insert_materialize(x136, y138, x_tile137, y_tile139, output_tan_q);
      input_q28.free_weight();
}
}

  output_conv_q.free_weight();
  output_relu_q.free_weight();
  output_q.free_weight();
}

