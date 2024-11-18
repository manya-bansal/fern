void my_fused_impl(const Array<float> a, Array<float> c, int64_t len258, int64_t len260){
  int64_t x259 = 0;
 int64_t x257 = x259;
 int64_t len256 = len258;
 int64_t x255 = x257;
 int64_t len254 = len256;
 Array<float> out_a_q = array_alloc<float>(x255, len254);
 Array<float> b_q = array_alloc<float>(x257, len256);
 for(int64_t x259 = c.idx;x259 < c.idx + c.size; x259+=len258){
    int64_t x257 = x259;
  int64_t len256 = len258;
  int64_t x255 = x257;
  int64_t len254 = len256;
  Array<float> a_q54 = a.array_query_no_materialize(x255, len254);
  Array<float> c_q = c.array_query_no_materialize(x259, len258);
    ispc::addi_ispc(a_q54.data, 0.000000f, len254, out_a_q.data);
    int64_t x261 = 0;
  for(int64_t x261 = b_q.idx;x261 < b_q.idx + b_q.size; x261+=len260){
      Array<float> b_q_q = b_q.array_query_no_materialize(x261, len260);
   Array<float> out_a_q55 = out_a.array_query_no_materialize(x261, len260);
      ispc::addi_ispc(out_a_q55.data, 0.000000f, len260, b_q_q);
}


    ispc::addi_ispc(b_q.data, 0.000000f, len258, c_q.data);
}

  out_a_q.destroy();
  b_q.destroy();
}

