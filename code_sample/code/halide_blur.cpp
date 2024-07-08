void my_fused_impl(const image<float> input, image<float> out2, int64_t y_tile46, int64_t x_tile52, int64_t y_tile53, int64_t o_s){
 for(int64_t y48 = 0;y48 < out2.logical_cols; y48+=y_tile46){
  cilk_scope {
   cilk_for(int64_t o = 0;o < out2.logical_rows; o+=o_s){
        int64_t i = 0;
    int64_t x47 = o + i;
    int64_t x_tile49 = 1;
    int64_t x42 = x47;
    int64_t y43 = y48;
    int64_t x_tile44 = 2+x_tile49;
    int64_t y_tile45 = y_tile46;
    image<float> out1_q = alloc_image(x42, y43, x_tile44, y_tile45);
    for(int64_t x50 = 0;x50 < out1_q.logical_rows; x50+=x_tile52){
     for(int64_t y51 = 0;y51 < out1_q.logical_cols; y51+=y_tile53){
            image<float> out1_q_q = out1_q.get_tile(x50, y51, x_tile52, y_tile53);
      image<float> input_q13 = input.get_tile(x50, y51, x_tile52, y_tile53 + 2);
            naive_blur_x_take_2_vec(input_q13, out1_q_q);
}
}

    for(int64_t i = o;i < o_s; i+=1){
          int64_t x47 = o + i;
     int64_t x_tile49 = 1;
     int64_t x42 = x47;
     int64_t y43 = y48;
     int64_t x_tile44 = 2+x_tile49;
     int64_t y_tile45 = y_tile46;
     image<float> out2_q = out2.get_tile(x47, y48, x_tile49, y_tile46);
     image<float> out1_q_q = out1_q.get_tile(x_tile44 - 1, y43, 1, y_tile45);
     image<float> input_q10 = input.get_tile(x42 + x_tile44 - 1, y43, 1, y_tile45 + 2);
          naive_blur_x_take_2_vec(input_q10, out1_q_q);
          naive_blur_y_take_2_vec(out1_q, out2_q);
     image<float> out1_q_move_src = out1_q.get_tile(1, y43, x_tile44 - 1, y_tile45);
          out1_q.insert_tile(0, y43, x_tile44 - 1, y_tile45, out1_q_move_src);
}

        out1_q.destroy();
}
   }
}
}

