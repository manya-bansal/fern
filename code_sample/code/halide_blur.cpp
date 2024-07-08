int64_t i = 0;
int64_t x4 = o + i;
int64_t x_tile6 = 1;
int64_t x0 = x4;
int64_t y1 = y5;
int64_t x_tile2 = 2+x_tile6;
int64_t y_tile3 = y_tile7;
image<float> out1_q = alloc_image(x0, y1, x_tile2, y_tile3);
image<float> out1_q_q = out1_q.get_tile(x8, y9, x_tile10, y_tile11);
image<float> input_q_3 = input.get_tile(x8, y9, x_tile10, y_tile11 + 2);
naive_blur_x_take_2_vec(input_q_3, out1_q_q);

int64_t x4 = int64_t o + i;
int64_t x_tile6 = 1;
int64_t x0 = x4;
int64_t y1 = y5;
int64_t x_tile2 = 2+x_tile6;
int64_t y_tile3 = y_tile7;
image<float> out2_q = out2.get_tile(x4, y5, x_tile6, y_tile7);
image<float> out1_q_q = out1_q.get_tile(x_tile2 - 1, y1, 1, y_tile3);
image<float> input_q_0 = input.get_tile(x0 + x_tile2 - 1, y1, 1, y_tile3 + 2);
naive_blur_x_take_2_vec(input_q_0, out1_q_q);
naive_blur_y_take_2_vec(out1_q_, out2_q);
image<float> out1_q_move_src = out1_q.get_tile(1, y1, x_tile2 - 1, y_tile3);
out1_q.insert_tile(0, y1, x_tile2 - 1, y_tile3, out1_q_move_src);

out1_q.destroy();
