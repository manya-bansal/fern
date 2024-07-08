int64_t x98 = 0;
int64_t y99 = 0;
image<float> output_q = output.get_tile(x98, y99, x_tile100, y_tile101);
image<float> input_q_19 = input.dummy_query(x98, y99, x_tile100, y_tile101);
sigmoid_processing(input_q_19, output_q);
input_q_19.destroy();

