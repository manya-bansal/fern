int64_t x146 = 0;
int64_t x142 = x144;
int64_t len143 = len145;
int64_t x141 = x142;
int64_t len140 = len143;
int64_t x138 = x141;
int64_t len139 = len140;
int64_t x137 = x138;
int64_t len136 = len139;
Array<float> add_1_q = array_alloc<float>(x137, len136);
for(int64_t x146 = add_7.idx;x146 < add_7.idx + add_7.size; x146+=len147){
  int64_t x142 = x144;
 int64_t len143 = len145;
 int64_t x141 = x142;
 int64_t len140 = len143;
 int64_t x138 = x141;
 int64_t len139 = len140;
 int64_t x137 = x138;
 int64_t len136 = len139;
 Array<float> c_q29 = c.array_query_no_materialize(x137, len136);
 Array<float> a_q28 = a.array_query_no_materialize(x138, len139);
 Array<float> b_q27 = b.array_query_no_materialize(x141, len140);
 Array<float> b_q26 = b.array_query_no_materialize(x142, len143);
 Array<float> a_q25 = a.array_query_no_materialize(x144, len145);
 Array<float> add_7_q = add_7.array_query_no_materialize(x146, len147);
 Array<float> add_6_q24 = add_6.array_query_no_materialize(x146, len147);
 Array<float> c_q23 = c.array_query_no_materialize(x146, len147);
  ispc::addi_ispc(c_q29, 0, len136, add_1_q);
  ispc::add_ispc(a_q28, add_1_q, len139, add_1_q);
  ispc::add_ispc(b_q27, add_1_q, len140, add_1_q);
  ispc::add_ispc(b_q26, add_1_q, len143, add_1_q);
  ispc::add_ispc(a_q25, add_1_q, len145, add_1_q);
  ispc::add_ispc(c_q23, add_1_q, len147, add_7_q);
}

add_1_q.destroy();
