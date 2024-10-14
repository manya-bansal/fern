void my_fused_impl(const Array<float> a, Array<float> c, int64_t len249, int64_t len247){
  int64_t x246 = 0;
 int64_t x244 = x246;
 int64_t len245 = len247;
 int64_t x242 = x244;
 int64_t len243 = len245;
 Array<float> out_a_q = array_alloc<float>(x242, len243);
 Array<float> b_q = array_alloc<float>(x244, len245);
 for(int64_t x246 = c.idx;x246 < c.idx + c.size; x246+=len247){
    int64_t x244 = x246;
  int64_t len245 = len247;
  int64_t x242 = x244;
  int64_t len243 = len245;
  Array<float> a_q48 = a.array_query_no_materialize(x242, len243);
  Array<float> c_q = c.array_query_no_materialize(x246, len247);
    ispc::addi_ispc(a_q48.data, 0, len243, out_a_q.data);
    int64_t x248 = 0;
  for(int64_t x248 = b_q.idx;x248 < b_q.idx + b_q.size; x248+=len249){
      Array<float> b_q_q = b_q.array_query_no_materialize(x248, len249);
   Array<float> out_a_q49 = out_a.array_query_no_materialize(x248, len249);
      ispc::addi_ispc(out_a_q49.data, 0, len249, b_q_q);
}


    ispc::addi_ispc(b_q.data, 0, len247, c_q.data);
}

  out_a_q.destroy();
  b_q.destroy();
}

