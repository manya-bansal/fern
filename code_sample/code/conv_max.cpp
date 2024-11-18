void my_fused_impl(const Weights<float> filter, const Weights<float> input, float* bias, Weights<float> output, int64_t stride_arg74, int64_t maxpool_dim83, int64_t x_tile84, int64_t y_tile87){
  int64_t x85 = 0;
 int64_t y86 = 0;
 int64_t x82 = maxpool_dim83*x85;
 int64_t y81 = maxpool_dim83*y86;
 int64_t x_tile80 = maxpool_dim83*x_tile84;
 int64_t y_tile79 = y_tile87*maxpool_dim83;
 int64_t x75 = x82;
 int64_t y76 = y81;
 int64_t x_tile77 = x_tile80;
 int64_t y_tile78 = y_tile79;
 Weights<float> output_conv_q = weight_alloc(x75, y76, x_tile77, y_tile78);
 Weights<float> output_relu_q = weight_alloc(x82, y81, x_tile80, y_tile79);
 for(int64_t x85 = output.W_start;x85 < output.W_start + output.LW; x85+=x_tile84){
  for(int64_t y86 = output.H_start;y86 < output.H_start + output.LH; y86+=y_tile87){
      int64_t x82 = maxpool_dim83*x85;
   int64_t y81 = maxpool_dim83*y86;
   int64_t x_tile80 = maxpool_dim83*x_tile84;
   int64_t y_tile79 = y_tile87*maxpool_dim83;
   int64_t x75 = x82;
   int64_t y76 = y81;
   int64_t x_tile77 = x_tile80;
   int64_t y_tile78 = y_tile79;
   Weights<float> input_q21 = input.query_materialize(x75 * stride_arg74, y76 * stride_arg74, x_tile77 + filter.H - 1, y_tile78 + filter.W - 1);
   Weights<float> output_q = output.query_materialize(x85, y86, x_tile84, y_tile87);
      conv_no_fern_mkl(input_q21, filter, bias, stride_arg74, output_conv_q);
      relu_material(output_conv_q, output_relu_q);
      maxpool(output_relu_q, maxpool_dim83, output_q);
      output.insert_materialize(x85, y86, x_tile84, y_tile87, output_q);
      input_q21.free_weight();
}
}

  output_conv_q.free_weight();
  output_relu_q.free_weight();
}

