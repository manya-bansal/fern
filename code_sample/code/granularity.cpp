void my_fused_impl(const Array<float> b, const Array<float> a, const Array<float> c, Array<float> r3, int64_t len120){
  int64_t x121 = 0;
 int64_t x119 = x121;
 int64_t len118 = len120;
 int64_t x116 = x121;
 int64_t len117 = len120;
 Array<float> r1_q = array_alloc<float>(x116, len117);
 Array<float> r2_q = array_alloc<float>(x119, len118);
 for(int64_t x121 = r3.idx;x121 < r3.idx + r3.size; x121+=len120){
    int64_t x119 = x121;
  int64_t len118 = len120;
  int64_t x116 = x121;
  int64_t len117 = len120;
  Array<float> a_q26 = a.array_query_no_materialize(x116, len117);
  Array<float> b_q25 = b.array_query_no_materialize(x116, len117);
  Array<float> c_q24 = c.array_query_no_materialize(x119, len118);
  Array<float> r3_q = r3.array_query_no_materialize(x121, len120);
    vadd(a_q26, b_q25, r1_q);
    vadd(r1_q, c_q24, r2_q);
    vadd(r1_q, r2_q, r3_q);
}

  r1_q.destroy();
  r2_q.destroy();
}

