void my_fused_impl(const Array<float> c, const Array<float> a, const Array<float> b, const Array<float> add_6, Array<float> add_7, int64_t len188, int64_t len186, int64_t x187){
  int64_t x189 = 0;
 int64_t x185 = x187;
 int64_t len184 = len186;
 int64_t x183 = x185;
 int64_t len182 = len184;
 int64_t x181 = x183;
 int64_t len180 = len182;
 int64_t x179 = x181;
 int64_t len178 = len180;
 Array<float> add_1_q = array_alloc<float>(x179, len178);
 for(int64_t x189 = add_7.idx;x189 < add_7.idx + add_7.size; x189+=len188){
    int64_t x185 = x187;
  int64_t len184 = len186;
  int64_t x183 = x185;
  int64_t len182 = len184;
  int64_t x181 = x183;
  int64_t len180 = len182;
  int64_t x179 = x181;
  int64_t len178 = len180;
  Array<float> c_q39 = c.array_query_no_materialize(x179, len178);
  Array<float> a_q38 = a.array_query_no_materialize(x181, len180);
  Array<float> b_q37 = b.array_query_no_materialize(x183, len182);
  Array<float> b_q36 = b.array_query_no_materialize(x185, len184);
  Array<float> a_q35 = a.array_query_no_materialize(x187, len186);
  Array<float> add_7_q = add_7.array_query_no_materialize(x189, len188);
  Array<float> add_6_q34 = add_6.array_query_no_materialize(x189, len188);
  Array<float> c_q33 = c.array_query_no_materialize(x189, len188);
    ispc::addi_ispc(c_q39.data, 0, len178, add_1_q.data);
    ispc::add_ispc(a_q38.data, add_1_q.data, len180, add_1_q.data);
    ispc::add_ispc(b_q37.data, add_1_q.data, len182, add_1_q.data);
    ispc::add_ispc(b_q36.data, add_1_q.data, len184, add_1_q.data);
    ispc::add_ispc(a_q35.data, add_1_q.data, len186, add_1_q.data);
    ispc::add_ispc(c_q33.data, add_1_q.data, len188, add_7_q.data);
}

  add_1_q.destroy();
}

