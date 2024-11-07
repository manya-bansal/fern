void my_fused_impl(const Array<float> a, Array<float> out, int64_t len2){
 for(int64_t x3 = out.idx;x3 < out.idx + out.size; x3+=len2){
    int64_t x1 = x3;
  int64_t len0 = len2;
  Array<float> b_q = array_alloc<float>(x1, len0);
  Array<float> a_q0 = a.array_query_no_materialize(x1, len0);
  Array<float> out_q = out.array_query_no_materialize(x3, len2);
    mock::addi(a_q0.data, 5
, len0, b_q.data);
    mock::addi(b_q.data, -3
, len2, out_q.data);
    b_q.destroy();
}
}

