void my_fused_impl(const image<float> input, image<float> final_out, int64_t x_tile114, int64_t y_tile115, int64_t y_tile111, int64_t o_s){
 for(int64_t y110 = 0;y110 < final_out.logical_cols; y110+=y_tile111){
  cilk_scope {
   cilk_for(int64_t o = 0;o < final_out.logical_rows; o+=o_s){
        int64_t i = 0;
    int64_t x108 = o + i;
    int64_t x_tile109 = 1;
    int64_t x105 = x108;
    int64_t y106 = y110;
    int64_t x_tile107 = x_tile109;
    int64_t y_tile104 = y_tile111;
    int64_t x101 = x105;
    int64_t y102 = y106;
    int64_t x_tile103 = x_tile107;
    int64_t y_tile100 = y_tile104;
    int64_t x88 = x105;
    int64_t y89 = y106;
    int64_t x_tile90 = x_tile107;
    int64_t y_tile91 = y_tile104;
    int64_t x97 = x101;
    int64_t y98 = y102;
    int64_t x_tile99 = x_tile103;
    int64_t y_tile96 = y_tile100;
    int64_t x92 = x97;
    int64_t y93 = y98;
    int64_t x_tile94 = x_tile99;
    int64_t y_tile95 = 6+y_tile96;
    image<float> gray_out_q = alloc_image(x88, y89, x_tile90, y_tile91);
    image<float> blur_y_temp_q = alloc_image(x92, y93, x_tile94, y_tile95);
    image<float> blur_x_temp_q = alloc_image(x97, y98, x_tile99, y_tile96);
    image<float> sharpen_temp_q = alloc_image(x101, y102, x_tile103, y_tile100);
    image<float> ratio_temp_q = alloc_image(x105, y106, x_tile107, y_tile104);
    for(int64_t x112 = 0;x112 < gray_out_q.logical_rows; x112+=x_tile114){
     for(int64_t y113 = 0;y113 < gray_out_q.logical_cols; y113+=y_tile115){
            image<float> gray_out_q_q = gray_out_q.get_tile(x112, y113, x_tile114, y_tile115);
      image<float> input_q23 = input.get_tile(x112, y113 * 3, x_tile114 + 6, y_tile115 * 3 + 18);
            gray_mine(input_q23, gray_out_q_q);
}
}

    for(int64_t i = o;i < o_s; i+=1){
          int64_t x108 = o + i;
     int64_t x_tile109 = 1;
     int64_t x105 = x108;
     int64_t y106 = y110;
     int64_t x_tile107 = x_tile109;
     int64_t y_tile104 = y_tile111;
     int64_t x101 = x105;
     int64_t y102 = y106;
     int64_t x_tile103 = x_tile107;
     int64_t y_tile100 = y_tile104;
     int64_t x88 = x105;
     int64_t y89 = y106;
     int64_t x_tile90 = x_tile107;
     int64_t y_tile91 = y_tile104;
     int64_t x97 = x101;
     int64_t y98 = y102;
     int64_t x_tile99 = x_tile103;
     int64_t y_tile96 = y_tile100;
     int64_t x92 = x97;
     int64_t y93 = y98;
     int64_t x_tile94 = x_tile99;
     int64_t y_tile95 = 6+y_tile96;
     image<float> final_out_q = final_out.get_tile(x108, y110, x_tile109, y_tile111);
     image<float> input_q19 = input.get_tile(x108, y110 * 3, x_tile109, y_tile111 * 3);
     image<float> gray_out_q_q = gray_out_q.get_tile(x_tile90 - 1, y89, 1, y_tile91);
     image<float> input_q20 = input.get_tile(x88 + x_tile90 - 1, y89 * 3, 1 + 6, y_tile91 * 3 + 18);
          gray_mine(input_q20, gray_out_q_q);
          naive_blur_y_unsharp_vec(gray_out_q, blur_y_temp_q);
          naive_blur_x_unsharp_vec(blur_y_temp_q, blur_x_temp_q);
          sharpen_vec(gray_out_q, blur_x_temp_q, sharpen_temp_q);
          ratio_vec(gray_out_q, sharpen_temp_q, ratio_temp_q);
          output_assign_mine(input_q19, ratio_temp_q, final_out_q);
     image<float> gray_out_q_move_src = gray_out_q.get_tile(1, y89, x_tile90 - 1, y_tile91);
          gray_out_q.insert_tile(0, y89, x_tile90 - 1, y_tile91, gray_out_q_move_src);
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

