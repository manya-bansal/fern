int64_t x94 = 0;
int64_t y97 = 0;
int64_t x91 = x94;
int64_t y92 = y97;
int64_t x_tile90 = x_tile96;
int64_t y_tile93 = y_tile95;
int64_t x88 = maxpool_dim89*x91;
int64_t y87 = maxpool_dim89*y92;
int64_t x_tile85 = maxpool_dim89*x_tile90;
int64_t y_tile86 = maxpool_dim89*y_tile93;
int64_t x81 = x88;
int64_t y82 = y87;
int64_t x_tile83 = x_tile85;
int64_t y_tile84 = y_tile86;
Weights<float> output_conv_q = weight_alloc(x81, y82, x_tile83, y_tile84);
Weights<float> output_relu_q = weight_alloc(x88, y87, x_tile85, y_tile86);
Weights<float> output_q = weight_alloc(x91, y92, x_tile90, y_tile93);
for(int64_t x94 = output_tan.W_start;x94 < output_tan.W_start + output_tan.LW; x94+=x_tile96){
 for(int64_t y97 = output_tan.H_start;y97 < output_tan.H_start + output_tan.LH; y97+=y_tile95){
    int64_t x91 = x94;
  int64_t y92 = y97;
  int64_t x_tile90 = x_tile96;
  int64_t y_tile93 = y_tile95;
  int64_t x88 = maxpool_dim89*x91;
  int64_t y87 = maxpool_dim89*y92;
  int64_t x_tile85 = maxpool_dim89*x_tile90;
  int64_t y_tile86 = maxpool_dim89*y_tile93;
  int64_t x81 = x88;
  int64_t y82 = y87;
  int64_t x_tile83 = x_tile85;
  int64_t y_tile84 = y_tile86;
  Weights<float> input_q18 = input.query_materialize(x81 * stride_arg80, y82 * stride_arg80, x_tile83 + filter.H - 1, y_tile84 + filter.W - 1);
  Weights<float> output_tan_q = output_tan.query_materialize(x94, y97, x_tile96, y_tile95);
    conv_no_fern_mkl(input_q18, filter, bias, stride_arg80, output_conv_q);
    relu_material(output_conv_q, output_relu_q);
    maxpool(output_relu_q, maxpool_dim89, output_q);
    tanh(output_q, output_tan_q);
    output_tan.insert_materialize(x94, y97, x_tile96, y_tile95, output_tan_q);
    input_q18.free_weight();
}
}

output_conv_q.free_weight();
output_relu_q.free_weight();
output_q.free_weight();
