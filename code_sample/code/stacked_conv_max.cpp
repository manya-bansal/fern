int64_t x44 = 0;
int64_t y42 = 0;
int64_t x37 = maxpool_dim41*x44;
int64_t y40 = y42*maxpool_dim41;
int64_t x_tile38 = maxpool_dim41*x_tile43;
int64_t y_tile39 = maxpool_dim41*y_tile45;
int64_t x33 = stride_arg36*x37;
int64_t y34 = y40*stride_arg36;
int64_t x_tile32 = -1+x_tile38+filter0.H;
int64_t y_tile35 = -1+y_tile39+filter0.W;
int64_t x26 = maxpool_dim31*x33;
int64_t y27 = maxpool_dim31*y34;
int64_t x_tile28 = maxpool_dim31*x_tile32;
int64_t y_tile29 = maxpool_dim31*y_tile35;
Weights<float> output_conv1_q = weight_alloc(x26, y27, x_tile28, y_tile29);
Weights<float> output_max1_q = weight_alloc(x33, y34, x_tile32, y_tile35);
Weights<float> output_conv2_q = weight_alloc(x37, y40, x_tile38, y_tile39);
for(int64_t x44 = output_t.W_start;x44 < output_t.W_start + output_t.LW; x44+=x_tile43){
 for(int64_t y42 = output_t.H_start;y42 < output_t.H_start + output_t.LH; y42+=y_tile45){
    int64_t x37 = maxpool_dim41*x44;
  int64_t y40 = y42*maxpool_dim41;
  int64_t x_tile38 = maxpool_dim41*x_tile43;
  int64_t y_tile39 = maxpool_dim41*y_tile45;
  int64_t x33 = stride_arg36*x37;
  int64_t y34 = y40*stride_arg36;
  int64_t x_tile32 = -1+x_tile38+filter0.H;
  int64_t y_tile35 = -1+y_tile39+filter0.W;
  int64_t x26 = maxpool_dim31*x33;
  int64_t y27 = maxpool_dim31*y34;
  int64_t x_tile28 = maxpool_dim31*x_tile32;
  int64_t y_tile29 = maxpool_dim31*y_tile35;
  Weights<float> input_q8 = input.query_materialize(x26 * stride_arg30, y27 * stride_arg30, x_tile28 + filter0.H - 1, y_tile29 + filter0.W - 1);
  Weights<float> output_t_q = output_t.query_materialize(x44, y42, x_tile43, y_tile45);
    conv_new_mkl(input_q8, filter0, bias, stride_arg30, output_conv1_q);
    maxpool(output_conv1_q, maxpool_dim31, output_max1_q);
    conv_new_mkl(output_max1_q, filter0, bias, stride_arg36, output_conv2_q);
    maxpool(output_conv2_q, maxpool_dim41, output_t_q);
    output_t.insert_materialize(x44, y42, x_tile43, y_tile45, output_t_q);
    input_q8.free_weight();
}
}

output_conv1_q.free_weight();
output_max1_q.free_weight();
output_conv2_q.free_weight();
