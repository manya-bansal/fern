void my_fused_impl(const Array<float> b, const Array<float> a, const Array<float> c, Array<float> r3, int64_t len140){
  int64_t x141 = 0;
 int64_t x139 = x141;
 int64_t len138 = len140;
 int64_t x136 = x141;
 int64_t len137 = len140;
 Array<float> r1_q = array_alloc<float>(x136, len137);
 Array<float> r2_q = array_alloc<float>(x139, len138);
 for(int64_t x141 = r3.idx;x141 < r3.idx + r3.size; x141+=len140){
    int64_t x139 = x141;
  int64_t len138 = len140;
  int64_t x136 = x141;
  int64_t len137 = len140;
  Array<float> a_q32 = a.array_query_no_materialize(x136, len137);
  Array<float> b_q31 = b.array_query_no_materialize(x136, len137);
  Array<float> c_q30 = c.array_query_no_materialize(x139, len138);
  Array<float> r3_q = r3.array_query_no_materialize(x141, len140);
    vadd(a_q32, b_q31, r1_q);
    vadd(r1_q, c_q30, r2_q);
    vadd(r1_q, r2_q, r3_q);
}

  r1_q.destroy();
  r2_q.destroy();
}

