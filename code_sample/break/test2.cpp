void my_fused_impl(const Array<float> a, Array<float> c, int64_t len15, int64_t len19){
  int64_t x26 = 0;
 int64_t len27 = c.idx + c.size;
 int64_t x24 = x26;
 int64_t len25 = len27;
 int64_t x22 = x24;
 int64_t len23 = len25;
 int64_t x20 = x22;
 int64_t len21 = len23;
 Array<float> b = array_alloc<float>(x22, len23);
  int64_t x14 = 0;
 int64_t x12 = x14;
 int64_t len13 = len15;
 Array<float> out_a_q = array_alloc<float>(x12, len13);
 for(int64_t x14 = b.idx;x14 < b.idx + b.size; x14+=len15){
    int64_t x12 = x14;
  int64_t len13 = len15;
  Array<float> a_q6 = a.array_query_no_materialize(x12, len13);
  Array<float> b_q = b.array_query_no_materialize(x14, len15);
    ispc::addi_ispc(a_q6.data, 0.000000f, len13, out_a_q.data);
    ispc::addi_ispc(out_a_q.data, 0.000000f, len15, b_q.data);
}

  out_a_q.destroy();

  int64_t x18 = 0;
 int64_t x16 = x18;
 int64_t len17 = len19;
 Array<float> b2_q = array_alloc<float>(x16, len17);
 for(int64_t x18 = c.idx;x18 < c.idx + c.size; x18+=len19){
    int64_t x16 = x18;
  int64_t len17 = len19;
  Array<float> b_q7 = b.array_query_no_materialize(x16, len17);
  Array<float> c_q = c.array_query_no_materialize(x18, len19);
    ispc::addi_ispc(b_q7.data, 0.000000f, len17, b2_q.data);
    ispc::addi_ispc(b2_q.data, 0.000000f, len19, c_q.data);
}

  b2_q.destroy();

}

