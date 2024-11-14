void my_fused_impl(const Array<float> a, Array<float> c, int64_t len14,
                   int64_t len18) {
  int64_t x27 = 0;
  int64_t len26 = c.idx + c.size;
  int64_t x25 = x27;
  int64_t len24 = len26;
  int64_t x23 = x25;
  int64_t len22 = len24;
  int64_t x21 = x23;
  int64_t len20 = len22;
  Array<float> b = array_alloc<float>(x23, len22);
  int64_t x15 = 0;
  int64_t x13 = x15;
  int64_t len12 = len14;
  Array<float> out_a_q = array_alloc<float>(x13, len12);
  for (int64_t x15 = b.idx; x15 < b.idx + b.size; x15 += len14) {
    int64_t x13 = x15;
    int64_t len12 = len14;
    Array<float> a_q6 = a.array_query_no_materialize(x13, len12);
    Array<float> b_q = b.array_query_no_materialize(x15, len14);
    ispc::addi_ispc(a_q6.data, 0, len12, out_a_q.data);
    ispc::addi_ispc(out_a_q.data, 0, len14, b_q.data);
  }

  out_a_q.destroy();

  int64_t x19 = 0;
  int64_t x17 = x19;
  int64_t len16 = len18;
  Array<float> b2_q = array_alloc<float>(x17, len16);
  for (int64_t x19 = c.idx; x19 < c.idx + c.size; x19 += len18) {
    int64_t x17 = x19;
    int64_t len16 = len18;
    Array<float> b_q7 = b.array_query_no_materialize(x17, len16);
    Array<float> c_q = c.array_query_no_materialize(x19, len18);
    ispc::addi_ispc(b_q7.data, 0, len16, b2_q.data);
    ispc::addi_ispc(b2_q.data, 0, len18, c_q.data);
  }

  b2_q.destroy();
}
