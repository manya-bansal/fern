void my_fused_impl(const Array<float> b, const Array<float> a, const Array<float> c, Array<float> r3, int64_t len78){
  int64_t x79 = 0;
 int64_t x77 = x79;
 int64_t len76 = len78;
 int64_t x74 = x79;
 int64_t len75 = len78;
 Array<float> r1_q = array_alloc<float>(x74, len75);
 Array<float> r2_q = array_alloc<float>(x77, len76);
 for(int64_t x79 = r3.idx;x79 < r3.idx + r3.size; x79+=len78){
    int64_t x77 = x79;
  int64_t len76 = len78;
  int64_t x74 = x79;
  int64_t len75 = len78;
  Array<float> a_q16 = a.array_query_no_materialize(x74, len75);
  Array<float> b_q15 = b.array_query_no_materialize(x74, len75);
  Array<float> c_q14 = c.array_query_no_materialize(x77, len76);
  Array<float> r3_q = r3.array_query_no_materialize(x79, len78);
    vadd(a_q16, b_q15, r1_q);
    vadd(r1_q, c_q14, r2_q);
    vadd(r1_q, r2_q, r3_q);
}

  r1_q.destroy();
  r2_q.destroy();
}

