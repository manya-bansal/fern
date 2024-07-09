void my_fused_impl(const Array<float> a, Array<float> c, int64_t len1,
                   int64_t len5) {
  int64_t x10 = 0;
  int64_t len11 = c.idx + c.size;
  int64_t x8 = x10;
  int64_t len9 = len11;
  int64_t x6 = x8;
  int64_t len7 = len9;
  Array<float> out_a = array_alloc<float>(x6, len7);
  int64_t x0 = 0;
  for (int64_t x0 = out_a.idx; x0 < out_a.idx + out_a.size; x0 += len1) {
    Array<float> out_a_q = out_a.array_query_no_materialize(x0, len1);
    Array<float> a_q2 = a.array_query_no_materialize(x0, len1);
    ispc::addi_ispc(a_q2.data, 0, len1, out_a_q.data);
  }

  int64_t x4 = 0;
  int64_t x2 = x4;
  int64_t len3 = len5;
  Array<float> b_q = array_alloc<float>(x2, len3);
  for (int64_t x4 = c.idx; x4 < c.idx + c.size; x4 += len5) {
    int64_t x2 = x4;
    int64_t len3 = len5;
    Array<float> out_a_q3 = out_a.array_query_no_materialize(x2, len3);
    Array<float> c_q = c.array_query_no_materialize(x4, len5);
    ispc::addi_ispc(out_a_q3.data, 0, len3, b_q.data);
    ispc::addi_ispc(b_q.data, 0, len5, c_q.data);
  }

  b_q.destroy();
}
