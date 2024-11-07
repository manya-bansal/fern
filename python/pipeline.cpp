void my_fused_impl(const Array<float> a, Array<float> out, int64_t len0){
 for(int64_t x1 = out.idx;x1 < out.idx + out.size; x1+=len0){
    Array<float> out_q = out.array_query_no_materialize(x1, len0);
  Array<float> a_q0 = a.array_query_no_materialize(x1, len0);
    mock::addi(a_q0.data, 1, len0, out_q.data);
}
}

