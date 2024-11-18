void my_fused_impl(const Array<float> c, const Array<float> a, const Array<float> b, const Array<float> add_6, Array<float> add_7, int64_t len206, int64_t x207, int64_t len208){
  int64_t x209 = 0;
 int64_t x205 = x207;
 int64_t len204 = len206;
 int64_t x203 = x205;
 int64_t len202 = len204;
 int64_t x201 = x203;
 int64_t len200 = len202;
 int64_t x199 = x201;
 int64_t len198 = len200;
 Array<float> add_1_q = array_alloc<float>(x199, len198);
 for(int64_t x209 = add_7.idx;x209 < add_7.idx + add_7.size; x209+=len208){
    int64_t x205 = x207;
  int64_t len204 = len206;
  int64_t x203 = x205;
  int64_t len202 = len204;
  int64_t x201 = x203;
  int64_t len200 = len202;
  int64_t x199 = x201;
  int64_t len198 = len200;
  Array<float> c_q45 = c.array_query_no_materialize(x199, len198);
  Array<float> a_q44 = a.array_query_no_materialize(x201, len200);
  Array<float> b_q43 = b.array_query_no_materialize(x203, len202);
  Array<float> b_q42 = b.array_query_no_materialize(x205, len204);
  Array<float> a_q41 = a.array_query_no_materialize(x207, len206);
  Array<float> add_7_q = add_7.array_query_no_materialize(x209, len208);
  Array<float> add_6_q40 = add_6.array_query_no_materialize(x209, len208);
  Array<float> c_q39 = c.array_query_no_materialize(x209, len208);
    ispc::addi_ispc(c_q45.data, 0.000000f, len198, add_1_q.data);
    ispc::add_ispc(a_q44.data, add_1_q.data, len200, add_1_q.data);
    ispc::add_ispc(b_q43.data, add_1_q.data, len202, add_1_q.data);
    ispc::add_ispc(b_q42.data, add_1_q.data, len204, add_1_q.data);
    ispc::add_ispc(a_q41.data, add_1_q.data, len206, add_1_q.data);
    ispc::add_ispc(c_q39.data, add_1_q.data, len208, add_7_q.data);
}

  add_1_q.destroy();
}

