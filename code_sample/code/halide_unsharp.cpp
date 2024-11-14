void my_fused_impl(const image<float> input, image<float> final_out, int64_t y_tile69, int64_t o_s, int64_t x_tile72, int64_t y_tile73){
 for(int64_t y67 = 0;y67 < final_out.logical_cols; y67+=y_tile69){
  cilk_scope {
   cilk_for(int64_t o = 0;o < final_out.logical_rows; o+=o_s){
        int64_t i = 0;
    int64_t x66 = o + i;
    int64_t x_tile68 = 1;
    int64_t x62 = x66;
    int64_t y63 = y67;
    int64_t x_tile64 = x_tile68;
    int64_t y_tile65 = y_tile69;
    int64_t x59 = x62;
    int64_t y58 = y63;
    int64_t x_tile60 = x_tile64;
    int64_t y_tile61 = y_tile65;
    int64_t x46 = x62;
    int64_t y47 = y63;
    int64_t x_tile48 = x_tile64;
    int64_t y_tile49 = y_tile65;
    int64_t x55 = x59;
    int64_t y54 = y58;
    int64_t x_tile56 = x_tile60;
    int64_t y_tile57 = y_tile61;
    int64_t x50 = x55;
    int64_t y51 = y54;
    int64_t x_tile52 = x_tile56;
    int64_t y_tile53 = 6+y_tile57;
    image<float> gray_out_q = alloc_image(x46, y47, x_tile48, y_tile49);
    image<float> blur_y_temp_q = alloc_image(x50, y51, x_tile52, y_tile53);
    image<float> blur_x_temp_q = alloc_image(x55, y54, x_tile56, y_tile57);
    image<float> sharpen_temp_q = alloc_image(x59, y58, x_tile60, y_tile61);
    image<float> ratio_temp_q = alloc_image(x62, y63, x_tile64, y_tile65);
    for(int64_t x70 = 0;x70 < gray_out_q.logical_rows; x70+=x_tile72){
     for(int64_t y71 = 0;y71 < gray_out_q.logical_cols; y71+=y_tile73){
            image<float> gray_out_q_q = gray_out_q.get_tile(x70, y71, x_tile72, y_tile73);
      image<float> input_q13 = input.get_tile(x70, y71 * 3, x_tile72 + 6, y_tile73 * 3 + 18);
            gray_mine(input_q13, gray_out_q_q);
}
}

    for(int64_t i = o;i < o_s; i+=1){
          int64_t x66 = o + i;
     int64_t x_tile68 = 1;
     int64_t x62 = x66;
     int64_t y63 = y67;
     int64_t x_tile64 = x_tile68;
     int64_t y_tile65 = y_tile69;
     int64_t x59 = x62;
     int64_t y58 = y63;
     int64_t x_tile60 = x_tile64;
     int64_t y_tile61 = y_tile65;
     int64_t x46 = x62;
     int64_t y47 = y63;
     int64_t x_tile48 = x_tile64;
     int64_t y_tile49 = y_tile65;
     int64_t x55 = x59;
     int64_t y54 = y58;
     int64_t x_tile56 = x_tile60;
     int64_t y_tile57 = y_tile61;
     int64_t x50 = x55;
     int64_t y51 = y54;
     int64_t x_tile52 = x_tile56;
     int64_t y_tile53 = 6+y_tile57;
     image<float> final_out_q = final_out.get_tile(x66, y67, x_tile68, y_tile69);
     image<float> input_q9 = input.get_tile(x66, y67 * 3, x_tile68, y_tile69 * 3);
     image<float> gray_out_q_q = gray_out_q.get_tile(x_tile48 - 1, y47, 1, y_tile49);
     image<float> input_q10 = input.get_tile(x46 + x_tile48 - 1, y47 * 3, 1 + 6, y_tile49 * 3 + 18);
          gray_mine(input_q10, gray_out_q_q);
          naive_blur_y_unsharp_vec(gray_out_q, blur_y_temp_q);
          naive_blur_x_unsharp_vec(blur_y_temp_q, blur_x_temp_q);
          sharpen_vec(gray_out_q, blur_x_temp_q, sharpen_temp_q);
          ratio_vec(gray_out_q, sharpen_temp_q, ratio_temp_q);
          output_assign_mine(input_q9, ratio_temp_q, final_out_q);
     image<float> gray_out_q_move_src = gray_out_q.get_tile(1, y47, x_tile48 - 1, y_tile49);
          gray_out_q.insert_tile(0, y47, x_tile48 - 1, y_tile49, gray_out_q_move_src);
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

