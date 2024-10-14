void my_fused_impl(const Array<float> a, Array<float> c, int64_t len260, int64_t len262, int64_t len264){
  int64_t x261 = 0;
 int64_t x259 = x261;
 int64_t len258 = len260;
 int64_t x257 = x259;
 int64_t len256 = len258;
 int64_t x255 = x257;
 int64_t len254 = len256;
 Array<float> out_a_q = array_alloc<float>(x255, len254);
 Array<float> b_q = array_alloc<float>(x257, len256);
 Array<float> b2_q = array_alloc<float>(x259, len258);
 for(int64_t x261 = c.idx;x261 < c.idx + c.size; x261+=len260){
    int64_t x259 = x261;
  int64_t len258 = len260;
  int64_t x257 = x259;
  int64_t len256 = len258;
  int64_t x255 = x257;
  int64_t len254 = len256;
  Array<float> a_q51 = a.array_query_no_materialize(x255, len254);
  Array<float> c_q = c.array_query_no_materialize(x261, len260);
    ispc::addi_ispc(a_q51.data, 0, len254, out_a_q.data);
    int64_t x265 = 0;
  Array<float> b2_q_q = array_alloc<float>(x263, len262);
  for(int64_t x265 = b2_q.idx;x265 < b2_q.idx + b2_q.size; x265+=len264){
      Array<float> out_a_q53 = out_a.array_query_no_materialize(x263, len262);
   Array<float> b2_q_q = b2_q.array_query_no_materialize(x265, len264);
   Array<float> b_q52 = b.array_query_no_materialize(x265, len264);
      ispc::addi_ispc(out_a_q53.data, 0, len262, b2_q_q);
      ispc::addi_ispc(b_q52.data, 0, len264, b2_q_q);
}

    b2_q_q.destroy();

    ispc::addi_ispc(b2_q.data, 0, len260, c_q.data);
}

  out_a_q.destroy();
  b_q.destroy();
  b2_q.destroy();
}

