void my_fused_impl(const Array<float> lat2, const Array<float> lon2, Array<float> a){
  int64_t x197 = 0;
 int64_t x195 = x197;
 int64_t len194 = len196;
 int64_t x192 = x195;
 int64_t len193 = len194;
 int64_t x190 = x192;
 int64_t len191 = len193;
 int64_t x188 = x190;
 int64_t len189 = len191;
 int64_t x186 = x188;
 int64_t len187 = len189;
 int64_t x179 = x188;
 int64_t len178 = len189;
 int64_t len196 = len187;
 int64_t x184 = x186;
 int64_t len185 = len187;
 int64_t x182 = x184;
 int64_t len183 = len185;
 int64_t x180 = x182;
 int64_t len181 = len183;
 int64_t x166 = x180;
 int64_t len167 = len181;
 int64_t x177 = x179;
 int64_t len176 = len178;
 int64_t x175 = x177;
 int64_t len174 = len176;
 int64_t x172 = x175;
 int64_t len173 = len174;
 int64_t x170 = x172;
 int64_t len171 = len173;
 int64_t x168 = x170;
 int64_t len169 = len171;
 int64_t x164 = x168;
 int64_t len165 = len169;
 Array<float> dlat_q = array_alloc<float>(x164, len165);
 Array<float> dlon_q = array_alloc<float>(x166, len167);
 Array<float> a2_q = array_alloc<float>(x186, len187);
 for(int64_t x197 = a.idx;x197 < a.idx + a.size; x197+=len196){
    int64_t x195 = x197;
  int64_t len194 = len196;
  int64_t x192 = x195;
  int64_t len193 = len194;
  int64_t x190 = x192;
  int64_t len191 = len193;
  int64_t x188 = x190;
  int64_t len189 = len191;
  int64_t x186 = x188;
  int64_t len187 = len189;
  int64_t x179 = x188;
  int64_t len178 = len189;
  int64_t len196 = len187;
  int64_t x184 = x186;
  int64_t len185 = len187;
  int64_t x182 = x184;
  int64_t len183 = len185;
  int64_t x180 = x182;
  int64_t len181 = len183;
  int64_t x166 = x180;
  int64_t len167 = len181;
  int64_t x177 = x179;
  int64_t len176 = len178;
  int64_t x175 = x177;
  int64_t len174 = len176;
  int64_t x172 = x175;
  int64_t len173 = len174;
  int64_t x170 = x172;
  int64_t len171 = len173;
  int64_t x168 = x170;
  int64_t len169 = len171;
  int64_t x164 = x168;
  int64_t len165 = len169;
  Array<float> lat2_q38 = lat2.array_query_no_materialize(x164, len165);
  Array<float> lon2_q37 = lon2.array_query_no_materialize(x166, len167);
  Array<float> a_q36 = a.array_query_no_materialize(x186, len187);
  Array<float> a_q = a.array_query_no_materialize(x197, len196);
    ispc::subi_ispc(lat2_q38.data, 0.709843f, len165, dlat_q.data);
    ispc::subi_ispc(lon2_q37.data, -1.238922f, len167, dlon_q.data);
    ispc::divi_ispc(dlat_q.data, 2.000000f, len169, dlat_q.data);
    ispc::sin_ispc(dlat_q.data, len171, dlat_q.data);
    ispc::sin_ispc(dlat_q.data, len173, dlat_q.data);
    ispc::mul_ispc(dlat_q.data, dlat_q.data, len174, dlat_q.data);
    ispc::cos_ispc(dlat_q.data, len176, dlat_q.data);
    ispc::muli_ispc(dlat_q.data, 0.758464f, len178, dlat_q.data);
    ispc::divi_ispc(dlon_q.data, 2.000000f, len181, dlon_q.data);
    ispc::sin_ispc(dlon_q.data, len183, dlon_q.data);
    ispc::mul_ispc(dlon_q.data, dlon_q.data, len185, dlon_q.data);
    ispc::mul_ispc(a_q36.data, dlon_q.data, len187, a2_q.data);
    ispc::add_ispc(a2_q.data, dlat_q.data, len189, a2_q.data);
    ispc::sqrt_ispc(a2_q.data, len191, a2_q.data);
    ispc::asin_ispc(a2_q.data, len193, a2_q.data);
    ispc::muli_ispc(a2_q.data, 2.000000f, len194, a2_q.data);
    ispc::muli_ispc(a2_q.data, 3959.000000f, len196, a_q.data);
}

  dlat_q.destroy();
  dlon_q.destroy();
  a2_q.destroy();
}

