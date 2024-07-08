int64_t x94 = 0;
int64_t y96 = 0;
int64_t x90 = x94;
int64_t y91 = y96;
int64_t x_tile92 = x_tile95;
int64_t y_tile89 = y_tile97;
int64_t x87 = x90*maxpool_dim93;
int64_t y86 = y91*maxpool_dim93;
int64_t x_tile85 = maxpool_dim93*x_tile92;
int64_t y_tile88 = y_tile89*maxpool_dim93;
int64_t x81 = x87;
int64_t y82 = y86;
int64_t x_tile83 = x_tile85;
int64_t y_tile84 = y_tile88;
Weights<float> output_conv_q = weight_alloc(x81, y82, x_tile83, y_tile84);
Weights<float> output_relu_q = weight_alloc(x87, y86, x_tile85, y_tile88);
Weights<float> output_q = weight_alloc(x90, y91, x_tile92, y_tile89);
for(int64_t x94 = output_tan.W_start;x94 < output_tan.W_start + output_tan.LW; x94+=x_tile95){
 for(int64_t y96 = output_tan.H_start;y96 < output_tan.H_start + output_tan.LH; y96+=y_tile97){
    int64_t x90 = x94;
  int64_t y91 = y96;
  int64_t x_tile92 = x_tile95;
  int64_t y_tile89 = y_tile97;
  int64_t x87 = x90*maxpool_dim93;
  int64_t y86 = y91*maxpool_dim93;
  int64_t x_tile85 = maxpool_dim93*x_tile92;
  int64_t y_tile88 = y_tile89*maxpool_dim93;
  int64_t x81 = x87;
  int64_t y82 = y86;
  int64_t x_tile83 = x_tile85;
  int64_t y_tile84 = y_tile88;
  Weights<float> input_q18 = input.query_materialize(x81 * stride_arg80, y82 * stride_arg80, x_tile83 + filter.H - 1, y_tile84 + filter.W - 1);
  Weights<float> output_tan_q = output_tan.query_materialize(x94, y96, x_tile95, y_tile97);
    conv_no_fern_mkl(input_q18, filter, bias, stride_arg80, output_conv_q);
    relu_material(output_conv_q, output_relu_q);
    maxpool(output_relu_q, maxpool_dim93, output_q);
    tanh(output_q, output_tan_q);
    output_tan.insert_materialize(x94, y96, x_tile95, y_tile97, output_tan_q);
    input_q18.free_weight();
}
}

output_conv_q.free_weight();
output_relu_q.free_weight();
output_q.free_weight();
