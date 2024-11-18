void my_fused_impl(const Array<float> a, Array<float> c, int64_t len268, int64_t len270, int64_t len272){
  int64_t x269 = 0;
 int64_t x267 = x269;
 int64_t len266 = len268;
 int64_t x265 = x267;
 int64_t len264 = len266;
 int64_t x263 = x265;
 int64_t len262 = len264;
 Array<float> out_a_q = array_alloc<float>(x263, len262);
 Array<float> b_q = array_alloc<float>(x265, len264);
 Array<float> b2_q = array_alloc<float>(x267, len266);
 for(int64_t x269 = c.idx;x269 < c.idx + c.size; x269+=len268){
    int64_t x267 = x269;
  int64_t len266 = len268;
  int64_t x265 = x267;
  int64_t len264 = len266;
  int64_t x263 = x265;
  int64_t len262 = len264;
  Array<float> a_q56 = a.array_query_no_materialize(x263, len262);
  Array<float> c_q = c.array_query_no_materialize(x269, len268);
    ispc::addi_ispc(a_q56.data, 0.000000f, len262, out_a_q.data);
    int64_t x273 = 0;
  Array<float> b2_q_q = array_alloc<float>(x271, len270);
  for(int64_t x273 = b2_q.idx;x273 < b2_q.idx + b2_q.size; x273+=len272){
      Array<float> out_a_q58 = out_a.array_query_no_materialize(x271, len270);
   Array<float> b2_q_q = b2_q.array_query_no_materialize(x273, len272);
   Array<float> b_q57 = b.array_query_no_materialize(x273, len272);
      ispc::addi_ispc(out_a_q58.data, 0.000000f, len270, b2_q_q);
      ispc::addi_ispc(b_q57.data, 0.000000f, len272, b2_q_q);
}

    b2_q_q.destroy();

    ispc::addi_ispc(b2_q.data, 0.000000f, len268, c_q.data);
}

  out_a_q.destroy();
  b_q.destroy();
  b2_q.destroy();
}

