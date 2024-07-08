void my_fused_impl(const image<float> input, image<float> out2, int64_t x_tile8, int64_t y_tile9, int64_t o_s, int64_t x_tile6, int64_t y_tile7){
 for(int64_t y5 = 0;y5 < out2.logical_cols; y5+=y_tile7){
  cilk_scope {
   cilk_for(int64_t o = 0;o < out2.logical_rows; o+=o_s){
        int64_t i = 0;
    int64_t x4 = o + i;
    int64_t x_tile6 = 1;
    int64_t x2 = x4;
    int64_t y3 = y5;
    int64_t x_tile0 = 2+x_tile6;
    int64_t y_tile1 = y_tile7;
    image<float> out1_q = alloc_image(x2, y3, x_tile0, y_tile1);
    for(int64_t x10 = 0;x10 < out1_q.logical_rows; x10+=x_tile8){
     for(int64_t y11 = 0;y11 < out1_q.logical_cols; y11+=y_tile9){
            image<float> out1_q_q = out1_q.get_tile(x10, y11, x_tile8, y_tile9);
      image<float> input_q3 = input.get_tile(x10, y11, x_tile8, y_tile9 + 2);
            naive_blur_x_take_2_vec(input_q3, out1_q_q);
}
}

    for(int64_t i = o;i < o_s; i+=1){
          int64_t x4 = o + i;
     int64_t x_tile6 = 1;
     int64_t x2 = x4;
     int64_t y3 = y5;
     int64_t x_tile0 = 2+x_tile6;
     int64_t y_tile1 = y_tile7;
     image<float> out2_q = out2.get_tile(x4, y5, x_tile6, y_tile7);
     image<float> out1_q_q = out1_q.get_tile(x_tile0 - 1, y3, 1, y_tile1);
     image<float> input_q0 = input.get_tile(x2 + x_tile0 - 1, y3, 1, y_tile1 + 2);
          naive_blur_x_take_2_vec(input_q0, out1_q_q);
          naive_blur_y_take_2_vec(out1_q, out2_q);
     image<float> out1_q_move_src = out1_q.get_tile(1, y3, x_tile0 - 1, y_tile1);
          out1_q.insert_tile(0, y3, x_tile0 - 1, y_tile1, out1_q_move_src);
}

        out1_q.destroy();
}
   }
}
}

