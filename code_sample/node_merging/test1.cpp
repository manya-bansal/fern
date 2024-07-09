void my_fused_impl(const Array<float> a, Array<float> c, int64_t len2, int64_t x3, int64_t len4){
  int64_t x5 = 0;
 int64_t x1 = x5;
 int64_t len0 = len4;
 Array<float> out_a_q = array_alloc<float>(x1, len0);
 Array<float> b_q = array_alloc<float>(x3, len2);
 for(int64_t x5 = c.idx;x5 < c.idx + c.size; x5+=len4){
    int64_t x1 = x5;
  int64_t len0 = len4;
  Array<float> a_q0 = a.array_query_no_materialize(x1, len0);
  Array<float> c_q = c.array_query_no_materialize(x5, len4);
    ispc::addi_ispc(a_q0.data, 0, len0, out_a_q.data);
    ispc::addi_ispc(out_a_q.data, 0, len2, b_q.data);
    ispc::addi_ispc(out_a_q.data, 0, len4, c_q.data);
}

  out_a_q.destroy();
  b_q.destroy();
}

