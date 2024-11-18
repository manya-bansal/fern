void my_fused_impl(const Array<float> d_in, Array<float> d_out_2, int64_t len44){
  int64_t x45 = 0;
 int64_t x42 = x45;
 int64_t len43 = len44;
 Array<float> d_out_1_q = array_alloc<float>(x42, len43);
 for(int64_t x45 = d_out_2.idx;x45 < d_out_2.idx + d_out_2.size; x45+=len44){
    int64_t x42 = x45;
  int64_t len43 = len44;
  Array<float> d_in_q10 = d_in.array_query_no_materialize(x42, len43);
  Array<float> d_out_2_q = d_out_2.array_query_no_materialize(x45, len44);
    kernel_1_whole(d_in_q10.data, d_out_1_q.data);
    kernel_2_whole(d_out_1_q.data, d_out_2_q.data);
}

  d_out_1_q.destroy();
}

