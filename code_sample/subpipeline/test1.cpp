void my_fused_impl(const Array<float> a, Array<float> c, int64_t len5,
                   int64_t len7) {
  int64_t x4 = 0;
  int64_t x2 = x4;
  int64_t len3 = len5;
  int64_t x0 = x2;
  int64_t len1 = len3;
  Array<float> out_a_q = array_alloc<float>(x0, len1);
  Array<float> b_q = array_alloc<float>(x2, len3);
  for (int64_t x4 = c.idx; x4 < c.idx + c.size; x4 += len5) {
    int64_t x2 = x4;
    int64_t len3 = len5;
    int64_t x0 = x2;
    int64_t len1 = len3;
    Array<float> a_q0 = a.array_query_no_materialize(x0, len1);
    Array<float> c_q = c.array_query_no_materialize(x4, len5);
    ispc::addi_ispc(a_q0.data, 0, len1, out_a_q.data);
    int64_t x6 = 0;
    for (int64_t x6 = b_q.idx; x6 < b_q.idx + b_q.size; x6 += len7) {
      Array<float> b_q_q = b_q.array_query_no_materialize(x6, len7);
      Array<float> out_a_q1 = out_a.array_query_no_materialize(x6, len7);
      ispc::addi_ispc(out_a_q1.data, 0, len7, b_q_q);
    }

    ispc::addi_ispc(b_q.data, 0, len5, c_q.data);
  }

  out_a_q.destroy();
  b_q.destroy();
}
