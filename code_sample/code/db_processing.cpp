void my_fused_impl(const image<float> input, image<float> output, int64_t x_tile100, int64_t y_tile101){
  int64_t x98 = 0;
 int64_t y99 = 0;
 for(int64_t x98 = 0;x98 < output.logical_rows; x98+=x_tile100){
  for(int64_t y99 = 0;y99 < output.logical_cols; y99+=y_tile101){
      image<float> output_q = output.get_tile(x98, y99, x_tile100, y_tile101);
   image<float> input_q19 = input.dummy_query(x98, y99, x_tile100, y_tile101);
      sigmoid_processing(input_q19, output_q);
      input_q19.destroy();
}
}

}

