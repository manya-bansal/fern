void my_fused_impl(const Array<float> a, Array<float> c, int64_t len257, int64_t len259, int64_t len261){
  int64_t x256 = 0;
 int64_t x254 = x256;
 int64_t len255 = len257;
 int64_t x252 = x254;
 int64_t len253 = len255;
 int64_t x250 = x252;
 int64_t len251 = len253;
 Array<float> out_a_q = array_alloc<float>(x250, len251);
 Array<float> b_q = array_alloc<float>(x252, len253);
 Array<float> b2_q = array_alloc<float>(x254, len255);
 for(int64_t x256 = c.idx;x256 < c.idx + c.size; x256+=len257){
    int64_t x254 = x256;
  int64_t len255 = len257;
  int64_t x252 = x254;
  int64_t len253 = len255;
  int64_t x250 = x252;
  int64_t len251 = len253;
  Array<float> a_q50 = a.array_query_no_materialize(x250, len251);
  Array<float> c_q = c.array_query_no_materialize(x256, len257);
    ispc::addi_ispc(a_q50.data, 0, len251, out_a_q.data);
    int64_t x260 = 0;
  Array<float> b2_q_q = array_alloc<float>(x258, len259);
  for(int64_t x260 = b2_q.idx;x260 < b2_q.idx + b2_q.size; x260+=len261){
      Array<float> out_a_q52 = out_a.array_query_no_materialize(x258, len259);
   Array<float> b2_q_q = b2_q.array_query_no_materialize(x260, len261);
   Array<float> b_q51 = b.array_query_no_materialize(x260, len261);
      ispc::addi_ispc(out_a_q52.data, 0, len259, b2_q_q);
      ispc::addi_ispc(b_q51.data, 0, len261, b2_q_q);
}

    b2_q_q.destroy();

    ispc::addi_ispc(b2_q.data, 0, len257, c_q.data);
}

  out_a_q.destroy();
  b_q.destroy();
  b2_q.destroy();
}

