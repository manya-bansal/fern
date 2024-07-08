int64_t x22 = 0;
int64_t y23 = 0;
int64_t x19 = x22*maxpool_dim21;
int64_t y18 = y23*maxpool_dim21;
int64_t x_tile20 = maxpool_dim21*x_tile24;
int64_t y_tile17 = y_tile25*maxpool_dim21;
int64_t x14 = x19;
int64_t y15 = y18;
int64_t x_tile16 = x_tile20;
int64_t y_tile13 = y_tile17;
Weights<float> output_conv_q = weight_alloc(x14, y15, x_tile16, y_tile13);
Weights<float> output_relu_q = weight_alloc(x19, y18, x_tile20, y_tile17);
for(int64_t x22 = output.W_start;x22 < output.W_start + output.LW; x22+=x_tile24){
 for(int64_t y23 = output.H_start;y23 < output.H_start + output.LH; y23+=y_tile25){
    int64_t x19 = x22*maxpool_dim21;
  int64_t y18 = y23*maxpool_dim21;
  int64_t x_tile20 = maxpool_dim21*x_tile24;
  int64_t y_tile17 = y_tile25*maxpool_dim21;
  int64_t x14 = x19;
  int64_t y15 = y18;
  int64_t x_tile16 = x_tile20;
  int64_t y_tile13 = y_tile17;
  Weights<float> input_q5 = input.query_materialize(x14 * stride_arg12, y15 * stride_arg12, x_tile16 + filter.H - 1, y_tile13 + filter.W - 1);
  Weights<float> output_q = output.query_materialize(x22, y23, x_tile24, y_tile25);
    conv_no_fern_mkl(input_q5, filter, bias, stride_arg12, output_conv_q);
    relu_material(output_conv_q, output_relu_q);
    maxpool(output_relu_q, maxpool_dim21, output_q);
    output.insert_materialize(x22, y23, x_tile24, y_tile25, output_q);
    input_q5.free_weight();
}
}

output_conv_q.free_weight();
output_relu_q.free_weight();
