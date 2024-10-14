void my_fused_impl(const Array<float> c, const Array<float> a, const Array<float> b, const Array<float> add_6, Array<float> add_7, int64_t x206, int64_t len207, int64_t len209){
  int64_t x208 = 0;
 int64_t x204 = x206;
 int64_t len205 = len207;
 int64_t x202 = x204;
 int64_t len203 = len205;
 int64_t x200 = x202;
 int64_t len201 = len203;
 int64_t x199 = x200;
 int64_t len198 = len201;
 Array<float> add_1_q = array_alloc<float>(x199, len198);
 for(int64_t x208 = add_7.idx;x208 < add_7.idx + add_7.size; x208+=len209){
    int64_t x204 = x206;
  int64_t len205 = len207;
  int64_t x202 = x204;
  int64_t len203 = len205;
  int64_t x200 = x202;
  int64_t len201 = len203;
  int64_t x199 = x200;
  int64_t len198 = len201;
  Array<float> c_q45 = c.array_query_no_materialize(x199, len198);
  Array<float> a_q44 = a.array_query_no_materialize(x200, len201);
  Array<float> b_q43 = b.array_query_no_materialize(x202, len203);
  Array<float> b_q42 = b.array_query_no_materialize(x204, len205);
  Array<float> a_q41 = a.array_query_no_materialize(x206, len207);
  Array<float> add_7_q = add_7.array_query_no_materialize(x208, len209);
  Array<float> add_6_q40 = add_6.array_query_no_materialize(x208, len209);
  Array<float> c_q39 = c.array_query_no_materialize(x208, len209);
    ispc::addi_ispc(c_q45.data, 0, len198, add_1_q.data);
    ispc::add_ispc(a_q44.data, add_1_q.data, len201, add_1_q.data);
    ispc::add_ispc(b_q43.data, add_1_q.data, len203, add_1_q.data);
    ispc::add_ispc(b_q42.data, add_1_q.data, len205, add_1_q.data);
    ispc::add_ispc(a_q41.data, add_1_q.data, len207, add_1_q.data);
    ispc::add_ispc(c_q39.data, add_1_q.data, len209, add_7_q.data);
}

  add_1_q.destroy();
}

