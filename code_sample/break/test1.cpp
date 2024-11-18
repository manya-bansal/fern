void my_fused_impl(const Array<float> a, Array<float> c, int64_t len0, int64_t len4){
  int64_t x11 = 0;
 int64_t len10 = c.idx + c.size;
 int64_t x9 = x11;
 int64_t len8 = len10;
 int64_t x7 = x9;
 int64_t len6 = len8;
 Array<float> out_a = array_alloc<float>(x7, len6);
  int64_t x1 = 0;
 for(int64_t x1 = out_a.idx;x1 < out_a.idx + out_a.size; x1+=len0){
    Array<float> out_a_q = out_a.array_query_no_materialize(x1, len0);
  Array<float> a_q2 = a.array_query_no_materialize(x1, len0);
    ispc::addi_ispc(a_q2.data, 0.000000f, len0, out_a_q.data);
}


  int64_t x5 = 0;
 int64_t x3 = x5;
 int64_t len2 = len4;
 Array<float> b_q = array_alloc<float>(x3, len2);
 for(int64_t x5 = c.idx;x5 < c.idx + c.size; x5+=len4){
    int64_t x3 = x5;
  int64_t len2 = len4;
  Array<float> out_a_q3 = out_a.array_query_no_materialize(x3, len2);
  Array<float> c_q = c.array_query_no_materialize(x5, len4);
    ispc::addi_ispc(out_a_q3.data, 0.000000f, len2, b_q.data);
    ispc::addi_ispc(b_q.data, 0.000000f, len4, c_q.data);
}

  b_q.destroy();

}

