void my_fused_impl(const Weights<float> filter, const Weights<float> input, float* bias, Weights<float> output_tan, int64_t maxpool_dim93, int64_t stride_arg80, int64_t maxpool_dim93, int64_t y_tile94, int64_t x_tile96){
  int64_t x95 = 0;
 int64_t y97 = 0;
 int64_t x91 = x95;
 int64_t y90 = y97;
 int64_t x_tile89 = x_tile96;
 int64_t y_tile92 = y_tile94;
 int64_t x86 = maxpool_dim93*x91;
 int64_t y85 = y90*maxpool_dim93;
 int64_t x_tile87 = maxpool_dim93*x_tile89;
 int64_t y_tile88 = y_tile92*maxpool_dim93;
 int64_t x81 = x86;
 int64_t y82 = y85;
 int64_t x_tile83 = x_tile87;
 int64_t y_tile84 = y_tile88;
 Weights<float> output_conv_q = weight_alloc(x81, y82, x_tile83, y_tile84);
 Weights<float> output_relu_q = weight_alloc(x86, y85, x_tile87, y_tile88);
 Weights<float> output_q = weight_alloc(x91, y90, x_tile89, y_tile92);
 for(int64_t x95 = output_tan.W_start;x95 < output_tan.W_start + output_tan.LW; x95+=x_tile96){
  for(int64_t y97 = output_tan.H_start;y97 < output_tan.H_start + output_tan.LH; y97+=y_tile94){
      int64_t x91 = x95;
   int64_t y90 = y97;
   int64_t x_tile89 = x_tile96;
   int64_t y_tile92 = y_tile94;
   int64_t x86 = maxpool_dim93*x91;
   int64_t y85 = y90*maxpool_dim93;
   int64_t x_tile87 = maxpool_dim93*x_tile89;
   int64_t y_tile88 = y_tile92*maxpool_dim93;
   int64_t x81 = x86;
   int64_t y82 = y85;
   int64_t x_tile83 = x_tile87;
   int64_t y_tile84 = y_tile88;
   Weights<float> input_q18 = input.query_materialize(x81 * stride_arg80, y82 * stride_arg80, x_tile83 + filter.H - 1, y_tile84 + filter.W - 1);
   Weights<float> output_tan_q = output_tan.query_materialize(x95, y97, x_tile96, y_tile94);
      conv_no_fern_mkl(input_q18, filter, bias, stride_arg80, output_conv_q);
      relu_material(output_conv_q, output_relu_q);
      maxpool(output_relu_q, maxpool_dim93, output_q);
      tanh(output_q, output_tan_q);
      output_tan.insert_materialize(x95, y97, x_tile96, y_tile94, output_tan_q);
      input_q18.free_weight();
}
}

  output_conv_q.free_weight();
  output_relu_q.free_weight();
  output_q.free_weight();
}

