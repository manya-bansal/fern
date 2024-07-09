void my_fused_impl(const Array<float> out_a, Array<float> b2, int64_t len12) {
  int64_t x13 = 0;
  int64_t x11 = x13;
  int64_t len10 = len12;
  int64_t x9 = x13;
  int64_t len8 = len12;
  int64_t x15 = x9;
  int64_t merge0 = x9 + len8;
  x15 = std::min(x15, x11 + 6);
  merge0 = std::max(merge0, x11 + 6 + len10);
  int64_t len14 = merge0 - x15;

  Array<float> out_a = array_alloc<float>(x15, len14);
  Array<float> b_q = array_alloc<float>(x9, len8);
  Array<float> c_q = array_alloc<float>(x11, len10);
  for (int64_t x13 = b2.idx; x13 < b2.idx + b2.size; x13 += len12) {
    int64_t x11 = x13;
    int64_t len10 = len12;
    int64_t x9 = x13;
    int64_t len8 = len12;
    int64_t x15 = x9;
    int64_t merge1 = x9 + len8;
    x15 = std::min(x15, x11 + 6);
    merge1 = std::max(merge1, x11 + 6 + len10);
    int64_t len14 = merge1 - x15;

    Array<float> a_q2 = a.array_query_no_materialize(x15, len14);
    ispc::addi_ispc(a_q2.data, 0, len14, out_a.data);
    Array<float> out_a_q4 = out_a.array_query_no_materialize(x9, len8);
    Array<float> out_a_q3 = out_a.array_query_no_materialize(x11 + 6, len10);
    Array<float> b2_q = b2.array_query_no_materialize(x13, len12);
    ispc::addi_ispc(out_a_q4.data, 0, len8, b_q.data);
    ispc::addi_ispc(out_a_q3.data, 0, len10, c_q.data);
    ispc::add_ispc(b_q.data, c_q.data, len12, b2_q.data);
  }

  b_q.destroy();
  c_q.destroy();
}
