void my_fused_impl(const image<float> input, image<float> out2, int64_t o_s, int64_t y_tile69, int64_t x_tile72, int64_t y_tile73){
 for(int64_t y67 = 0;y67 < out2.logical_cols; y67+=y_tile69){
  cilk_scope {
   cilk_for(int64_t o = 0;o < out2.logical_rows; o+=o_s){
        int64_t i = 0;
    int64_t x66 = o + i;
    int64_t x_tile68 = 1;
    int64_t x62 = x66;
    int64_t y63 = y67;
    int64_t x_tile64 = 2+x_tile68;
    int64_t y_tile65 = y_tile69;
    image<float> out1_q = alloc_image(x62, y63, x_tile64, y_tile65);
    for(int64_t x70 = 0;x70 < out1_q.logical_rows; x70+=x_tile72){
     for(int64_t y71 = 0;y71 < out1_q.logical_cols; y71+=y_tile73){
            image<float> out1_q_q = out1_q.get_tile(x70, y71, x_tile72, y_tile73);
      image<float> input_q19 = input.get_tile(x70, y71, x_tile72, y_tile73 + 2);
            naive_blur_x_take_2_vec(input_q19, out1_q_q);
}
}

    for(int64_t i = o;i < o_s; i+=1){
          int64_t x66 = o + i;
     int64_t x_tile68 = 1;
     int64_t x62 = x66;
     int64_t y63 = y67;
     int64_t x_tile64 = 2+x_tile68;
     int64_t y_tile65 = y_tile69;
     image<float> out2_q = out2.get_tile(x66, y67, x_tile68, y_tile69);
     image<float> out1_q_q = out1_q.get_tile(x_tile64 - 1, y63, 1, y_tile65);
     image<float> input_q16 = input.get_tile(x62 + x_tile64 - 1, y63, 1, y_tile65 + 2);
          naive_blur_x_take_2_vec(input_q16, out1_q_q);
          naive_blur_y_take_2_vec(out1_q, out2_q);
     image<float> out1_q_move_src = out1_q.get_tile(1, y63, x_tile64 - 1, y_tile65);
          out1_q.insert_tile(0, y63, x_tile64 - 1, y_tile65, out1_q_move_src);
}

        out1_q.destroy();
}
   }
}
}

