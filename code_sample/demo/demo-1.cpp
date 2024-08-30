void my_fused_impl(const Array<float> d_in, Array<float> d_out_2, int64_t len3){
 int64_t x2 = 0;
 int64_t x0 = x2;
 int64_t len1 = len3;
 Array<float> d_out_1_q = array_alloc<float>(x0, len1);
 
 for(int64_t x2 = d_out_2.idx;x2 < d_out_2.idx + d_out_2.size; x2+=len3){
    int64_t x0 = x2;
    int64_t len1 = len3;
    Array<float> d_in_q0 = d_in.array_query_no_materialize(x0, len1);
    Array<float> d_out_2_q = d_out_2.array_query_no_materialize(x2, len3);
    kernel_1_whole(d_in_q0.data, d_out_1_q.data);
    kernel_2_whole(d_out_1_q.data, d_out_2_q.data);
}

  d_out_1_q.destroy();
}

