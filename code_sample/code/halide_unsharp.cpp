void my_fused_impl(const image<float> input, image<float> final_out, int64_t x_tile134, int64_t y_tile135, int64_t o_s, int64_t y_tile130){
 for(int64_t y131 = 0;y131 < final_out.logical_cols; y131+=y_tile130){
  cilk_scope {
   cilk_for(int64_t o = 0;o < final_out.logical_rows; o+=o_s){
        int64_t i = 0;
    int64_t x128 = o + i;
    int64_t x_tile129 = 1;
    int64_t x124 = x128;
    int64_t y125 = y131;
    int64_t x_tile126 = x_tile129;
    int64_t y_tile127 = y_tile130;
    int64_t x122 = x124;
    int64_t y121 = y125;
    int64_t x_tile120 = x_tile126;
    int64_t y_tile123 = y_tile127;
    int64_t x108 = x124;
    int64_t y109 = y125;
    int64_t x_tile110 = x_tile126;
    int64_t y_tile111 = y_tile127;
    int64_t x116 = x122;
    int64_t y118 = y121;
    int64_t x_tile117 = x_tile120;
    int64_t y_tile119 = y_tile123;
    int64_t x114 = x116;
    int64_t y113 = y118;
    int64_t x_tile115 = x_tile117;
    int64_t y_tile112 = 6+y_tile119;
    image<float> gray_out_q = alloc_image(x108, y109, x_tile110, y_tile111);
    image<float> blur_y_temp_q = alloc_image(x114, y113, x_tile115, y_tile112);
    image<float> blur_x_temp_q = alloc_image(x116, y118, x_tile117, y_tile119);
    image<float> sharpen_temp_q = alloc_image(x122, y121, x_tile120, y_tile123);
    image<float> ratio_temp_q = alloc_image(x124, y125, x_tile126, y_tile127);
    for(int64_t x132 = 0;x132 < gray_out_q.logical_rows; x132+=x_tile134){
     for(int64_t y133 = 0;y133 < gray_out_q.logical_cols; y133+=y_tile135){
            image<float> gray_out_q_q = gray_out_q.get_tile(x132, y133, x_tile134, y_tile135);
      image<float> input_q29 = input.get_tile(x132, y133 * 3, x_tile134 + 6, y_tile135 * 3 + 18);
            gray_mine(input_q29, gray_out_q_q);
}
}

    for(int64_t i = o;i < o_s; i+=1){
          int64_t x128 = o + i;
     int64_t x_tile129 = 1;
     int64_t x124 = x128;
     int64_t y125 = y131;
     int64_t x_tile126 = x_tile129;
     int64_t y_tile127 = y_tile130;
     int64_t x122 = x124;
     int64_t y121 = y125;
     int64_t x_tile120 = x_tile126;
     int64_t y_tile123 = y_tile127;
     int64_t x108 = x124;
     int64_t y109 = y125;
     int64_t x_tile110 = x_tile126;
     int64_t y_tile111 = y_tile127;
     int64_t x116 = x122;
     int64_t y118 = y121;
     int64_t x_tile117 = x_tile120;
     int64_t y_tile119 = y_tile123;
     int64_t x114 = x116;
     int64_t y113 = y118;
     int64_t x_tile115 = x_tile117;
     int64_t y_tile112 = 6+y_tile119;
     image<float> final_out_q = final_out.get_tile(x128, y131, x_tile129, y_tile130);
     image<float> input_q25 = input.get_tile(x128, y131 * 3, x_tile129, y_tile130 * 3);
     image<float> gray_out_q_q = gray_out_q.get_tile(x_tile110 - 1, y109, 1, y_tile111);
     image<float> input_q26 = input.get_tile(x108 + x_tile110 - 1, y109 * 3, 1 + 6, y_tile111 * 3 + 18);
          gray_mine(input_q26, gray_out_q_q);
          naive_blur_y_unsharp_vec(gray_out_q, blur_y_temp_q);
          naive_blur_x_unsharp_vec(blur_y_temp_q, blur_x_temp_q);
          sharpen_vec(gray_out_q, blur_x_temp_q, sharpen_temp_q);
          ratio_vec(gray_out_q, sharpen_temp_q, ratio_temp_q);
          output_assign_mine(input_q25, ratio_temp_q, final_out_q);
     image<float> gray_out_q_move_src = gray_out_q.get_tile(1, y109, x_tile110 - 1, y_tile111);
          gray_out_q.insert_tile(0, y109, x_tile110 - 1, y_tile111, gray_out_q_move_src);
}

        gray_out_q.destroy();
        blur_y_temp_q.destroy();
        blur_x_temp_q.destroy();
        sharpen_temp_q.destroy();
        ratio_temp_q.destroy();
}
   }
}
}

