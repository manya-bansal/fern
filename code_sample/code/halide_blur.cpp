void my_fused_impl(const image<float> input, image<float> out2, int64_t x_tile4, int64_t y_tile5){
 for(int64_t y6 = 0;y6 < out2.logical_cols; y6+=y_tile5){
  cilk_scope {
   cilk_for(int64_t o = 0;o < out2.logical_rows; o+=o_s){
        int64_t i = 0;
    int64_t x7 = o + i;
    int64_t x_tile4 = 1;
    int64_t x0 = x7;
    int64_t y1 = y6;
    int64_t x_tile2 = 2+x_tile4;
    int64_t y_tile3 = y_tile5;
    image<float> out1_q = alloc_image(x0, y1, x_tile2, y_tile3);
    for(int64_t x8 = 0;x8 < out1_q.logical_rows; x8+=x_tile10){
     for(int64_t y9 = 0;y9 < out1_q.logical_cols; y9+=y_tile11){
            image<float> out1_q_q = out1_q.get_tile(x8, y9, x_tile10, y_tile11);
      image<float> input_q3 = input.get_tile(x8, y9, x_tile10, y_tile11 + 2);
            naive_blur_x_take_2_vec(input_q3, out1_q_q);
}
}

    for(int64_t i = o;i < o_s; i+=1){
          int64_t x7 = o + i;
     int64_t x_tile4 = 1;
     int64_t x0 = x7;
     int64_t y1 = y6;
     int64_t x_tile2 = 2+x_tile4;
     int64_t y_tile3 = y_tile5;
     image<float> out2_q = out2.get_tile(x7, y6, x_tile4, y_tile5);
     image<float> out1_q_q = out1_q.get_tile(x_tile2 - 1, y1, 1, y_tile3);
     image<float> input_q0 = input.get_tile(x0 + x_tile2 - 1, y1, 1, y_tile3 + 2);
          naive_blur_x_take_2_vec(input_q0, out1_q_q);
          naive_blur_y_take_2_vec(out1_q, out2_q);
     image<float> out1_q_move_src = out1_q.get_tile(1, y1, x_tile2 - 1, y_tile3);
          out1_q.insert_tile(0, y1, x_tile2 - 1, y_tile3, out1_q_move_src);
}

        out1_q.destroy();
}
   }
}
}

