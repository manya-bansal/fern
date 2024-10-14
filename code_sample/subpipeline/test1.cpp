void my_fused_impl(const Array<float> a, Array<float> c, int64_t len250, int64_t len252){
  int64_t x251 = 0;
 int64_t x249 = x251;
 int64_t len248 = len250;
 int64_t x247 = x249;
 int64_t len246 = len248;
 Array<float> out_a_q = array_alloc<float>(x247, len246);
 Array<float> b_q = array_alloc<float>(x249, len248);
 for(int64_t x251 = c.idx;x251 < c.idx + c.size; x251+=len250){
    int64_t x249 = x251;
  int64_t len248 = len250;
  int64_t x247 = x249;
  int64_t len246 = len248;
  Array<float> a_q49 = a.array_query_no_materialize(x247, len246);
  Array<float> c_q = c.array_query_no_materialize(x251, len250);
    ispc::addi_ispc(a_q49.data, 0, len246, out_a_q.data);
    int64_t x253 = 0;
  for(int64_t x253 = b_q.idx;x253 < b_q.idx + b_q.size; x253+=len252){
      Array<float> b_q_q = b_q.array_query_no_materialize(x253, len252);
   Array<float> out_a_q50 = out_a.array_query_no_materialize(x253, len252);
      ispc::addi_ispc(out_a_q50.data, 0, len252, b_q_q);
}


    ispc::addi_ispc(b_q.data, 0, len250, c_q.data);
}

  out_a_q.destroy();
  b_q.destroy();
}

