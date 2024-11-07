void my_fused_impl(const Array<float> a, const Array<float> b, Array<float> out, int64_t len1){
 for(int64_t x0 = out.idx;x0 < out.idx + out.size; x0+=len1){
    Array<float> out_q = out.array_query(x0, len1);
  Array<float> b_q1 = b.array_query(x0, len1);
  Array<float> a_q0 = a.array_query(x0, len1);
    vadd(a_q0.data, b_q1.data, out_q.data);
    out.array_insert(x0, len1, out_q);
}
}

