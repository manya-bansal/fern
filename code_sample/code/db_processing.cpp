void my_fused_impl(const image<float> input, image<float> output, int64_t x_tile141, int64_t y_tile142){
  int64_t x143 = 0;
 int64_t y140 = 0;
 for(int64_t x143 = 0;x143 < output.logical_rows; x143+=x_tile141){
  for(int64_t y140 = 0;y140 < output.logical_cols; y140+=y_tile142){
      image<float> output_q = output.get_tile(x143, y140, x_tile141, y_tile142);
   image<float> input_q29 = input.dummy_query(x143, y140, x_tile141, y_tile142);
      sigmoid_processing(input_q29, output_q);
      input_q29.destroy();
}
}

}

