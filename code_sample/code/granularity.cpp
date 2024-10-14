void my_fused_impl(const Array<float> b, const Array<float> a, const Array<float> c, Array<float> r3, int64_t len141){
  int64_t x140 = 0;
 int64_t x138 = x140;
 int64_t len139 = len141;
 int64_t x136 = x140;
 int64_t len137 = len141;
 Array<float> r1_q = array_alloc<float>(x136, len137);
 Array<float> r2_q = array_alloc<float>(x138, len139);
 for(int64_t x140 = r3.idx;x140 < r3.idx + r3.size; x140+=len141){
    int64_t x138 = x140;
  int64_t len139 = len141;
  int64_t x136 = x140;
  int64_t len137 = len141;
  Array<float> a_q32 = a.array_query_no_materialize(x136, len137);
  Array<float> b_q31 = b.array_query_no_materialize(x136, len137);
  Array<float> c_q30 = c.array_query_no_materialize(x138, len139);
  Array<float> r3_q = r3.array_query_no_materialize(x140, len141);
    vadd(a_q32, b_q31, r1_q);
    vadd(r1_q, c_q30, r2_q);
    vadd(r1_q, r2_q, r3_q);
}

  r1_q.destroy();
  r2_q.destroy();
}

