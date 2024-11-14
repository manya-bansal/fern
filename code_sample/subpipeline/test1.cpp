void my_fused_impl(const Array<float> a, Array<float> c, int64_t len228,
                   int64_t len230) {
  int64_t x229 = 0;
  int64_t x227 = x229;
  int64_t len226 = len228;
  int64_t x225 = x227;
  int64_t len224 = len226;
  Array<float> out_a_q = array_alloc<float>(x225, len224);
  Array<float> b_q = array_alloc<float>(x227, len226);
  for (int64_t x229 = c.idx; x229 < c.idx + c.size; x229 += len228) {
    int64_t x227 = x229;
    int64_t len226 = len228;
    int64_t x225 = x227;
    int64_t len224 = len226;
    Array<float> a_q43 = a.array_query_no_materialize(x225, len224);
    Array<float> c_q = c.array_query_no_materialize(x229, len228);
    ispc::addi_ispc(a_q43.data, 0, len224, out_a_q.data);
    int64_t x231 = 0;
    for (int64_t x231 = b_q.idx; x231 < b_q.idx + b_q.size; x231 += len230) {
      Array<float> b_q_q = b_q.array_query_no_materialize(x231, len230);
      Array<float> out_a_q44 = out_a.array_query_no_materialize(x231, len230);
      ispc::addi_ispc(out_a_q44.data, 0, len230, b_q_q);
    }

    ispc::addi_ispc(b_q.data, 0, len228, c_q.data);
  }

  out_a_q.destroy();
  b_q.destroy();
}
