void my_fused_impl(const Array<float> lat2, const Array<float> lon2, Array<float> a, int64_t len180){
  int64_t x181 = 0;
 int64_t x179 = x181;
 int64_t len178 = len180;
 int64_t x177 = x179;
 int64_t len176 = len178;
 int64_t x174 = x177;
 int64_t len175 = len176;
 int64_t x172 = x174;
 int64_t len173 = len175;
 int64_t x170 = x172;
 int64_t len171 = len173;
 int64_t x163 = x172;
 int64_t len162 = len173;
 int64_t x168 = x170;
 int64_t len169 = len171;
 int64_t x166 = x168;
 int64_t len167 = len169;
 int64_t x164 = x166;
 int64_t len165 = len167;
 int64_t x150 = x164;
 int64_t len151 = len165;
 int64_t x160 = x163;
 int64_t len161 = len162;
 int64_t x158 = x160;
 int64_t len159 = len161;
 int64_t x156 = x158;
 int64_t len157 = len159;
 int64_t x154 = x156;
 int64_t len155 = len157;
 int64_t x152 = x154;
 int64_t len153 = len155;
 int64_t x149 = x152;
 int64_t len148 = len153;
 Array<float> a3_q = array_alloc<float>(x172, len173);
 Array<float> a4_q = array_alloc<float>(x174, len175);
 Array<float> a5_q = array_alloc<float>(x177, len176);
 Array<float> a6_q = array_alloc<float>(x179, len178);
 for(int64_t x181 = a.idx;x181 < a.idx + a.size; x181+=len180){
    int64_t x179 = x181;
  int64_t len178 = len180;
  int64_t x177 = x179;
  int64_t len176 = len178;
  int64_t x174 = x177;
  int64_t len175 = len176;
  int64_t x172 = x174;
  int64_t len173 = len175;
  int64_t x170 = x172;
  int64_t len171 = len173;
  int64_t x163 = x172;
  int64_t len162 = len173;
  int64_t x168 = x170;
  int64_t len169 = len171;
  int64_t x166 = x168;
  int64_t len167 = len169;
  int64_t x164 = x166;
  int64_t len165 = len167;
  int64_t x150 = x164;
  int64_t len151 = len165;
  int64_t x160 = x163;
  int64_t len161 = len162;
  int64_t x158 = x160;
  int64_t len159 = len161;
  int64_t x156 = x158;
  int64_t len157 = len159;
  int64_t x154 = x156;
  int64_t len155 = len157;
  int64_t x152 = x154;
  int64_t len153 = len155;
  int64_t x149 = x152;
  int64_t len148 = len153;
  Array<float> lat2_q32 = lat2.array_query_no_materialize(x149, len148);
  Array<float> lon2_q31 = lon2.array_query_no_materialize(x150, len151);
  Array<float> a_q30 = a.array_query_no_materialize(x170, len171);
  Array<float> a_q = a.array_query_no_materialize(x181, len180);
    ispc::subi_ispc(lat2_q32.data, 0.709843, len148, dlon_q.data);
    ispc::subi_ispc(lon2_q31.data, -1.23892, len151, dlat_q.data);
    ispc::divi_ispc(dlon_q.data, 2, len153, dlon_q.data);
    ispc::sin_ispc(dlon_q.data, len155, dlon_q.data);
    ispc::sin_ispc(dlon_q.data, len157, dlon_q.data);
    ispc::mul_ispc(dlon_q.data, dlon_q.data, len159, dlon_q.data);
    ispc::cos_ispc(dlon_q.data, len161, dlon_q.data);
    ispc::muli_ispc(dlon_q.data, 0.758464, len162, dlon_q.data);
    ispc::divi_ispc(dlat_q.data, 2, len165, dlat7_q.data);
    ispc::sin_ispc(dlat7_q.data, len167, dlat7_q.data);
    ispc::mul_ispc(dlat7_q.data, dlat7_q.data, len169, dlat7_q.data);
    ispc::mul_ispc(a_q30.data, dlat7_q.data, len171, dlat7_q.data);
    ispc::add_ispc(dlat7_q.data, dlon_q.data, len173, a3_q.data);
    ispc::sqrt_ispc(a3_q.data, len175, a4_q.data);
    ispc::asin_ispc(a4_q.data, len176, a5_q.data);
    ispc::muli_ispc(a5_q.data, 2, len178, a6_q.data);
    ispc::muli_ispc(a6_q.data, 3959, len180, a_q.data);
}

  a3_q.destroy();
  a4_q.destroy();
  a5_q.destroy();
  a6_q.destroy();
}

