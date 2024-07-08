void my_fused_impl(const Weights<float> filter, const Weights<float> input, float* bias, Weights<float> output_tan, int64_t stride_arg80, int64_t maxpool_dim93, int64_t maxpool_dim93, int64_t x_tile94, int64_t y_tile97){
  int64_t x95 = 0;
 int64_t y96 = 0;
 int64_t x89 = x95;
 int64_t y90 = y96;
 int64_t x_tile91 = x_tile94;
 int64_t y_tile92 = y_tile97;
 int64_t x88 = maxpool_dim93*x89;
 int64_t y87 = y90*maxpool_dim93;
 int64_t x_tile85 = maxpool_dim93*x_tile91;
 int64_t y_tile86 = maxpool_dim93*y_tile92;
 int64_t x81 = x88;
 int64_t y82 = y87;
 int64_t x_tile83 = x_tile85;
 int64_t y_tile84 = y_tile86;
 Weights<float> output_conv_q = weight_alloc(x81, y82, x_tile83, y_tile84);
 Weights<float> output_relu_q = weight_alloc(x88, y87, x_tile85, y_tile86);
 Weights<float> output_q = weight_alloc(x89, y90, x_tile91, y_tile92);
 for(int64_t x95 = output_tan.W_start;x95 < output_tan.W_start + output_tan.LW; x95+=x_tile94){
  for(int64_t y96 = output_tan.H_start;y96 < output_tan.H_start + output_tan.LH; y96+=y_tile97){
      int64_t x89 = x95;
   int64_t y90 = y96;
   int64_t x_tile91 = x_tile94;
   int64_t y_tile92 = y_tile97;
   int64_t x88 = maxpool_dim93*x89;
   int64_t y87 = y90*maxpool_dim93;
   int64_t x_tile85 = maxpool_dim93*x_tile91;
   int64_t y_tile86 = maxpool_dim93*y_tile92;
   int64_t x81 = x88;
   int64_t y82 = y87;
   int64_t x_tile83 = x_tile85;
   int64_t y_tile84 = y_tile86;
   Weights<float> input_q18 = input.query_materialize(x81 * stride_arg80, y82 * stride_arg80, x_tile83 + filter.H - 1, y_tile84 + filter.W - 1);
   Weights<float> output_tan_q = output_tan.query_materialize(x95, y96, x_tile94, y_tile97);
      conv_no_fern_mkl(input_q18, filter, bias, stride_arg80, output_conv_q);
      relu_material(output_conv_q, output_relu_q);
      maxpool(output_relu_q, maxpool_dim93, output_q);
      tanh(output_q, output_tan_q);
      output_tan.insert_materialize(x95, y96, x_tile94, y_tile97, output_tan_q);
      input_q18.free_weight();
}
}

  output_conv_q.free_weight();
  output_relu_q.free_weight();
  output_q.free_weight();
}

