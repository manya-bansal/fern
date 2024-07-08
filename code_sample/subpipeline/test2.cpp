void my_fused_impl(const Array<float> a, Array<float> c, int64_t len14, int64_t len16, int64_t len18){
  int64_t x15 = 0;
 int64_t x13 = x15;
 int64_t len12 = len14;
 int64_t x11 = x13;
 int64_t len10 = len12;
 int64_t x9 = x11;
 int64_t len8 = len10;
 Array<float> out_a_q = array_alloc<float>(x9, len8);
 Array<float> b_q = array_alloc<float>(x11, len10);
 Array<float> b2_q = array_alloc<float>(x13, len12);
 for(int64_t x15 = c.idx;x15 < c.idx + c.size; x15+=len14){
    int64_t x13 = x15;
  int64_t len12 = len14;
  int64_t x11 = x13;
  int64_t len10 = len12;
  int64_t x9 = x11;
  int64_t len8 = len10;
  Array<float> a_q2 = a.array_query_no_materialize(x9, len8);
  Array<float> c_q = c.array_query_no_materialize(x15, len14);
    ispc::addi_ispc(a_q2.data, 0, len8, out_a_q.data);
    int64_t x19 = 0;
  Array<float> b2_q_q = array_alloc<float>(x17, len16);
  for(int64_t x19 = b2_q.idx;x19 < b2_q.idx + b2_q.size; x19+=len18){
      Array<float> out_a_q4 = out_a.array_query_no_materialize(x17, len16);
   Array<float> b2_q_q = b2_q.array_query_no_materialize(x19, len18);
   Array<float> b_q3 = b.array_query_no_materialize(x19, len18);
      ispc::addi_ispc(out_a_q4.data, 0, len16, b2_q_q);
      ispc::addi_ispc(b_q3.data, 0, len18, b2_q_q);
}

    b2_q_q.destroy();

    ispc::addi_ispc(b2_q.data, 0, len14, c_q.data);
}

  out_a_q.destroy();
  b_q.destroy();
  b2_q.destroy();
}

