void my_fused_impl(const image<float> input, image<float> final_out, int64_t x_tile68, int64_t y_tile69){
 for(int64_t y67 = 0;y67 < final_out.logical_cols; y67+=y_tile69){
  cilk_scope {
   cilk_for(int64_t o = 0;o < final_out.logical_rows; o+=o_s){
        int64_t i = 0;
    int64_t x66 = o + i;
    int64_t x62 = x66;
    int64_t y64 = y67;
    int64_t x_tile63 = x_tile68;
    int64_t y_tile65 = y_tile69;
    int64_t x58 = x62;
    int64_t y59 = y64;
    int64_t x_tile60 = x_tile63;
    int64_t y_tile61 = y_tile65;
    int64_t x46 = x62;
    int64_t y48 = y64;
    int64_t x_tile47 = x_tile63;
    int64_t y_tile49 = y_tile65;
    int64_t x54 = x58;
    int64_t y55 = y59;
    int64_t x_tile56 = x_tile60;
    int64_t y_tile57 = y_tile61;
    int64_t x50 = x54;
    int64_t y51 = y55;
    int64_t x_tile52 = x_tile56;
    int64_t y_tile53 = 6+y_tile57;
    image<float> gray_out_q = alloc_image(x46, y48, x_tile47, y_tile49);
    image<float> blur_y_temp_q = alloc_image(x50, y51, x_tile52, y_tile53);
    image<float> blur_x_temp_q = alloc_image(x54, y55, x_tile56, y_tile57);
    image<float> sharpen_temp_q = alloc_image(x58, y59, x_tile60, y_tile61);
    image<float> ratio_temp_q = alloc_image(x62, y64, x_tile63, y_tile65);
    for(int64_t x70 = 0;x70 < gray_out_q.logical_rows; x70+=x_tile71){
     for(int64_t y72 = 0;y72 < gray_out_q.logical_cols; y72+=y_tile73){
            image<float> gray_out_q_q = gray_out_q.get_tile(x70, y72, x_tile71, y_tile73);
      image<float> input_q13 = input.get_tile(x70, y72 * 3, x_tile71 + 6, y_tile73 * 3 + 18);
            gray_mine(input_q13, gray_out_q_q);
}
}

    for(int64_t i = o;i < o_s; i+=1){
          int64_t x66 = o + i;
     int64_t x62 = x66;
     int64_t y64 = y67;
     int64_t x_tile63 = x_tile68;
     int64_t y_tile65 = y_tile69;
     int64_t x58 = x62;
     int64_t y59 = y64;
     int64_t x_tile60 = x_tile63;
     int64_t y_tile61 = y_tile65;
     int64_t x46 = x62;
     int64_t y48 = y64;
     int64_t x_tile47 = x_tile63;
     int64_t y_tile49 = y_tile65;
     int64_t x54 = x58;
     int64_t y55 = y59;
     int64_t x_tile56 = x_tile60;
     int64_t y_tile57 = y_tile61;
     int64_t x50 = x54;
     int64_t y51 = y55;
     int64_t x_tile52 = x_tile56;
     int64_t y_tile53 = 6+y_tile57;
     image<float> final_out_q = final_out.get_tile(x66, y67, x_tile68, y_tile69);
     image<float> input_q9 = input.get_tile(x66, y67 * 3, x_tile68, y_tile69 * 3);
     image<float> gray_out_q_q = gray_out_q.get_tile(x_tile47 - 1, y48, 1, y_tile49);
     image<float> input_q10 = input.get_tile(x46 + x_tile47 - 1, y48 * 3, 1 + 6, y_tile49 * 3 + 18);
          gray_mine(input_q10, gray_out_q_q);
          naive_blur_y_unsharp_vec(gray_out_q, blur_y_temp_q);
          naive_blur_x_unsharp_vec(blur_y_temp_q, blur_x_temp_q);
          sharpen_vec(gray_out_q, blur_x_temp_q, sharpen_temp_q);
          ratio_vec(gray_out_q, sharpen_temp_q, ratio_temp_q);
          output_assign_mine(input_q9, ratio_temp_q, final_out_q);
     image<float> gray_out_q_move_src = gray_out_q.get_tile(1, y48, x_tile47 - 1, y_tile49);
          gray_out_q.insert_tile(0, y48, x_tile47 - 1, y_tile49, gray_out_q_move_src);
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

