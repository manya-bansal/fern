int64_t x135 = 0;
int64_t x133 = x135;
int64_t len132 = len134;
int64_t x131 = x133;
int64_t len130 = len132;
int64_t x129 = x131;
int64_t len128 = len130;
int64_t x127 = x129;
int64_t len126 = len128;
int64_t x125 = x127;
int64_t len124 = len126;
int64_t x117 = x127;
int64_t len116 = len126;
int64_t len134 = len124;
int64_t x123 = x125;
int64_t len122 = len124;
int64_t x120 = x123;
int64_t len121 = len122;
int64_t x118 = x120;
int64_t len119 = len121;
int64_t x104 = x118;
int64_t len105 = len119;
int64_t x114 = x117;
int64_t len115 = len116;
int64_t x112 = x114;
int64_t len113 = len115;
int64_t x110 = x112;
int64_t len111 = len113;
int64_t x108 = x110;
int64_t len109 = len111;
int64_t x106 = x108;
int64_t len107 = len109;
int64_t x103 = x106;
int64_t len102 = len107;
Array<float> dlat_q = array_alloc<float>(x103, len102);
Array<float> dlon_q = array_alloc<float>(x104, len105);
Array<float> a2_q = array_alloc<float>(x125, len124);
for(int64_t x135 = a.idx;x135 < a.idx + a.size; x135+=len134){
  int64_t x133 = x135;
 int64_t len132 = len134;
 int64_t x131 = x133;
 int64_t len130 = len132;
 int64_t x129 = x131;
 int64_t len128 = len130;
 int64_t x127 = x129;
 int64_t len126 = len128;
 int64_t x125 = x127;
 int64_t len124 = len126;
 int64_t x117 = x127;
 int64_t len116 = len126;
 int64_t len134 = len124;
 int64_t x123 = x125;
 int64_t len122 = len124;
 int64_t x120 = x123;
 int64_t len121 = len122;
 int64_t x118 = x120;
 int64_t len119 = len121;
 int64_t x104 = x118;
 int64_t len105 = len119;
 int64_t x114 = x117;
 int64_t len115 = len116;
 int64_t x112 = x114;
 int64_t len113 = len115;
 int64_t x110 = x112;
 int64_t len111 = len113;
 int64_t x108 = x110;
 int64_t len109 = len111;
 int64_t x106 = x108;
 int64_t len107 = len109;
 int64_t x103 = x106;
 int64_t len102 = len107;
 Array<float> lat2_q22 = lat2.array_query_no_materialize(x103, len102);
 Array<float> lon2_q21 = lon2.array_query_no_materialize(x104, len105);
 Array<float> a_q20 = a.array_query_no_materialize(x125, len124);
 Array<float> a_q = a.array_query_no_materialize(x135, len134);
  ispc::subi_ispc(lat2_q22, 0.709843, len102, dlat_q);
  ispc::subi_ispc(lon2_q21, -1.23892, len105, dlon_q);
  ispc::divi_ispc(dlat_q, 2, len107, dlat_q);
  ispc::sin_ispc(dlat_q, len109, dlat_q);
  ispc::sin_ispc(dlat_q, len111, dlat_q);
  ispc::mul_ispc(dlat_q, dlat_q, len113, dlat_q);
  ispc::cos_ispc(dlat_q, len115, dlat_q);
  ispc::muli_ispc(dlat_q, 0.758464, len116, dlat_q);
  ispc::divi_ispc(dlon_q, 2, len119, dlon_q);
  ispc::sin_ispc(dlon_q, len121, dlon_q);
  ispc::mul_ispc(dlon_q, dlon_q, len122, dlon_q);
  ispc::mul_ispc(a_q20, dlon_q, len124, a2_q);
  ispc::add_ispc(a2_q, dlat_q, len126, a2_q);
  ispc::sqrt_ispc(a2_q, len128, a2_q);
  ispc::asin_ispc(a2_q, len130, a2_q);
  ispc::muli_ispc(a2_q, 2, len132, a2_q);
  ispc::muli_ispc(a2_q, 3959, len134, a_q);
}

dlat_q.destroy();
dlon_q.destroy();
a2_q.destroy();
