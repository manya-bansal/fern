void my_fused_impl(const image<float> input, image<float> output, int64_t x_tile162, int64_t y_tile163){
  int64_t x160 = 0;
 int64_t y161 = 0;
 for(int64_t x160 = 0;x160 < output.logical_rows; x160+=x_tile162){
  for(int64_t y161 = 0;y161 < output.logical_cols; y161+=y_tile163){
      image<float> output_q = output.get_tile(x160, y161, x_tile162, y_tile163);
   image<float> input_q35 = input.dummy_query(x160, y161, x_tile162, y_tile163);
      sigmoid_processing(input_q35, output_q);
      input_q35.destroy();
}
}

}

