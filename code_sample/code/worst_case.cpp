int64_t x147 = 0;
int64_t x143 = x145;
int64_t len142 = len144;
int64_t x141 = x143;
int64_t len140 = len142;
int64_t x139 = x141;
int64_t len138 = len140;
int64_t x137 = x139;
int64_t len136 = len138;
Array<float> add_1_q = array_alloc<float>(x137, len136);
for(int64_t x147 = add_7.idx;x147 < add_7.idx + add_7.size; x147+=len146){
  int64_t x143 = x145;
 int64_t len142 = len144;
 int64_t x141 = x143;
 int64_t len140 = len142;
 int64_t x139 = x141;
 int64_t len138 = len140;
 int64_t x137 = x139;
 int64_t len136 = len138;
 Array<float> c_q29 = c.array_query_no_materialize(x137, len136);
 Array<float> a_q28 = a.array_query_no_materialize(x139, len138);
 Array<float> b_q27 = b.array_query_no_materialize(x141, len140);
 Array<float> b_q26 = b.array_query_no_materialize(x143, len142);
 Array<float> a_q25 = a.array_query_no_materialize(x145, len144);
 Array<float> add_7_q = add_7.array_query_no_materialize(x147, len146);
 Array<float> add_6_q24 = add_6.array_query_no_materialize(x147, len146);
 Array<float> c_q23 = c.array_query_no_materialize(x147, len146);
  ispc::addi_ispc(c_q29, 0, len136, add_1_q);
  ispc::add_ispc(a_q28, add_1_q, len138, add_1_q);
  ispc::add_ispc(b_q27, add_1_q, len140, add_1_q);
  ispc::add_ispc(b_q26, add_1_q, len142, add_1_q);
  ispc::add_ispc(a_q25, add_1_q, len144, add_1_q);
  ispc::add_ispc(c_q23, add_1_q, len146, add_7_q);
}

add_1_q.destroy();
