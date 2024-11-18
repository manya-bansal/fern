void my_fused_impl(const Array<float> out_a, Array<float> b2, int64_t len58){
  int64_t x59 = 0;
 int64_t x57 = x59;
 int64_t len56 = len58;
 int64_t x55 = x59;
 int64_t len54 = len58;
  int64_t x61 = x55;
 int64_t merge0 = x55 + len54;
 x61 = std::min(x61, x57 + 6);
 merge0 = std::max(merge0, x57 + 6 + len56);
 int64_t len60 = merge0 - x61;

 Array<float> out_a = array_alloc<float>(x61, len60);
 Array<float> b_q = array_alloc<float>(x55, len54);
 Array<float> c_q = array_alloc<float>(x57, len56);
 for(int64_t x59 = b2.idx;x59 < b2.idx + b2.size; x59+=len58){
    int64_t x57 = x59;
  int64_t len56 = len58;
  int64_t x55 = x59;
  int64_t len54 = len58;
    int64_t x61 = x55;
  int64_t merge1 = x55 + len54;
  x61 = std::min(x61, x57 + 6);
  merge1 = std::max(merge1, x57 + 6 + len56);
  int64_t len60 = merge1 - x61;

  Array<float> a_q13 = a.array_query_no_materialize(x61, len60);
    ispc::addi_ispc(a_q13.data, 0.000000f, len60, out_a.data);
  Array<float> out_a_q15 = out_a.array_query_no_materialize(x55, len54);
  Array<float> out_a_q14 = out_a.array_query_no_materialize(x57 + 6, len56);
  Array<float> b2_q = b2.array_query_no_materialize(x59, len58);
    ispc::addi_ispc(out_a_q15.data, 0.000000f, len54, b_q.data);
    ispc::addi_ispc(out_a_q14.data, 0.000000f, len56, c_q.data);
    ispc::add_ispc(b_q.data, c_q.data, len58, b2_q.data);
}

  b_q.destroy();
  c_q.destroy();
}

