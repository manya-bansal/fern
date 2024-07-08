void my_fused_impl(const Array<float> a, Array<float> c, int64_t len240, int64_t len242, int64_t len238){
  int64_t x239 = 0;
 int64_t x237 = x239;
 int64_t len236 = len238;
 int64_t x235 = x237;
 int64_t len234 = len236;
 int64_t x233 = x235;
 int64_t len232 = len234;
 Array<float> out_a_q = array_alloc<float>(x233, len232);
 Array<float> b_q = array_alloc<float>(x235, len234);
 Array<float> b2_q = array_alloc<float>(x237, len236);
 for(int64_t x239 = c.idx;x239 < c.idx + c.size; x239+=len238){
    int64_t x237 = x239;
  int64_t len236 = len238;
  int64_t x235 = x237;
  int64_t len234 = len236;
  int64_t x233 = x235;
  int64_t len232 = len234;
  Array<float> a_q45 = a.array_query_no_materialize(x233, len232);
  Array<float> c_q = c.array_query_no_materialize(x239, len238);
    ispc::addi_ispc(a_q45.data, 0, len232, out_a_q.data);
    int64_t x243 = 0;
  Array<float> b2_q_q = array_alloc<float>(x241, len240);
  for(int64_t x243 = b2_q.idx;x243 < b2_q.idx + b2_q.size; x243+=len242){
      Array<float> out_a_q47 = out_a.array_query_no_materialize(x241, len240);
   Array<float> b2_q_q = b2_q.array_query_no_materialize(x243, len242);
   Array<float> b_q46 = b.array_query_no_materialize(x243, len242);
      ispc::addi_ispc(out_a_q47.data, 0, len240, b2_q_q);
      ispc::addi_ispc(b_q46.data, 0, len242, b2_q_q);
}

    b2_q_q.destroy();

    ispc::addi_ispc(b2_q.data, 0, len238, c_q.data);
}

  out_a_q.destroy();
  b_q.destroy();
  b2_q.destroy();
}

