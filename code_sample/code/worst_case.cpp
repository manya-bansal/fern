void my_fused_impl(const Array<float> c, const Array<float> a, const Array<float> b, const Array<float> add_6, Array<float> add_7, int64_t len202, int64_t x203, int64_t len204){
  int64_t x205 = 0;
 int64_t x201 = x203;
 int64_t len200 = len202;
 int64_t x199 = x201;
 int64_t len198 = len200;
 int64_t x197 = x199;
 int64_t len196 = len198;
 int64_t x195 = x197;
 int64_t len194 = len196;
 Array<float> add_1_q = array_alloc<float>(x195, len194);
 for(int64_t x205 = add_7.idx;x205 < add_7.idx + add_7.size; x205+=len204){
    int64_t x201 = x203;
  int64_t len200 = len202;
  int64_t x199 = x201;
  int64_t len198 = len200;
  int64_t x197 = x199;
  int64_t len196 = len198;
  int64_t x195 = x197;
  int64_t len194 = len196;
  Array<float> c_q44 = c.array_query_no_materialize(x195, len194);
  Array<float> a_q43 = a.array_query_no_materialize(x197, len196);
  Array<float> b_q42 = b.array_query_no_materialize(x199, len198);
  Array<float> b_q41 = b.array_query_no_materialize(x201, len200);
  Array<float> a_q40 = a.array_query_no_materialize(x203, len202);
  Array<float> add_7_q = add_7.array_query_no_materialize(x205, len204);
  Array<float> add_6_q39 = add_6.array_query_no_materialize(x205, len204);
  Array<float> c_q38 = c.array_query_no_materialize(x205, len204);
    ispc::addi_ispc(c_q44.data, 0, len194, add_1_q.data);
    ispc::add_ispc(a_q43.data, add_1_q.data, len196, add_1_q.data);
    ispc::add_ispc(b_q42.data, add_1_q.data, len198, add_1_q.data);
    ispc::add_ispc(b_q41.data, add_1_q.data, len200, add_1_q.data);
    ispc::add_ispc(a_q40.data, add_1_q.data, len202, add_1_q.data);
    ispc::add_ispc(c_q38.data, add_1_q.data, len204, add_7_q.data);
}

  add_1_q.destroy();
}

