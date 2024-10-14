void my_fused_impl(const Array<float> a, Array<float> c, int64_t len50, int64_t len48, int64_t x49){
  int64_t x51 = 0;
 int64_t x47 = x51;
 int64_t len46 = len50;
 Array<float> out_a_q = array_alloc<float>(x47, len46);
 Array<float> b_q = array_alloc<float>(x49, len48);
 for(int64_t x51 = c.idx;x51 < c.idx + c.size; x51+=len50){
    int64_t x47 = x51;
  int64_t len46 = len50;
  Array<float> a_q11 = a.array_query_no_materialize(x47, len46);
  Array<float> c_q = c.array_query_no_materialize(x51, len50);
    ispc::addi_ispc(a_q11.data, 0, len46, out_a_q.data);
    ispc::addi_ispc(out_a_q.data, 0, len48, b_q.data);
    ispc::addi_ispc(out_a_q.data, 0, len50, c_q.data);
}

  out_a_q.destroy();
  b_q.destroy();
}

