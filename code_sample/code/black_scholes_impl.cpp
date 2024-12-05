void my_fused_impl(const Array<float> strike_price, const Array<float> stock_price, const Array<float> volatility, Array<float> out, int64_t len196){
  int64_t x197 = 0;
 int64_t x195 = x197;
 int64_t len194 = len196;
 int64_t x187 = x197;
 int64_t len186 = len196;
 int64_t x191 = x195;
 int64_t len190 = len194;
 int64_t x193 = x195;
 int64_t len192 = len194;
 int64_t x189 = x191;
 int64_t len188 = len190;
 int64_t x181 = x189;
 int64_t len180 = len188;
 int64_t x183 = x189;
 int64_t len182 = len188;
 int64_t x185 = x187;
 int64_t len184 = len186;
 int64_t x179 = x183;
 int64_t len178 = len182;
 int64_t x177 = x179;
 int64_t len176 = len178;
 int64_t x169 = x179;
 int64_t len168 = len178;
 int64_t x175 = x177;
 int64_t len174 = len176;
 int64_t x173 = x175;
 int64_t len172 = len174;
 int64_t x171 = x173;
 int64_t len170 = len172;
 int64_t x167 = x169;
 int64_t len166 = len168;
 Array<float> s_over_k_q = array_alloc<float>(x167, len166);
 Array<float> log_res_q = array_alloc<float>(x169, len168);
 Array<float> vol_sq_q = array_alloc<float>(x171, len170);
 Array<float> vol_sq_half_q = array_alloc<float>(x173, len172);
 Array<float> vol_sq_half_plusr_q = array_alloc<float>(x175, len174);
 Array<float> vol_inter_q = array_alloc<float>(x177, len176);
 Array<float> d1_inter_q = array_alloc<float>(x179, len178);
 Array<float> vol_sqrt_T_q = array_alloc<float>(x181, len180);
 Array<float> d1_q = array_alloc<float>(x183, len182);
 Array<float> d1_cdf_q = array_alloc<float>(x185, len184);
 Array<float> part1_q = array_alloc<float>(x187, len186);
 Array<float> d2_q = array_alloc<float>(x189, len188);
 Array<float> d2_cdf_q = array_alloc<float>(x191, len190);
 Array<float> k_scaled_q = array_alloc<float>(x193, len192);
 Array<float> part2_q = array_alloc<float>(x195, len194);
 for(int64_t x197 = out.idx;x197 < out.idx + out.size; x197+=len196){
    int64_t x195 = x197;
  int64_t len194 = len196;
  int64_t x187 = x197;
  int64_t len186 = len196;
  int64_t x191 = x195;
  int64_t len190 = len194;
  int64_t x193 = x195;
  int64_t len192 = len194;
  int64_t x189 = x191;
  int64_t len188 = len190;
  int64_t x181 = x189;
  int64_t len180 = len188;
  int64_t x183 = x189;
  int64_t len182 = len188;
  int64_t x185 = x187;
  int64_t len184 = len186;
  int64_t x179 = x183;
  int64_t len178 = len182;
  int64_t x177 = x179;
  int64_t len176 = len178;
  int64_t x169 = x179;
  int64_t len168 = len178;
  int64_t x175 = x177;
  int64_t len174 = len176;
  int64_t x173 = x175;
  int64_t len172 = len174;
  int64_t x171 = x173;
  int64_t len170 = len172;
  int64_t x167 = x169;
  int64_t len166 = len168;
  Array<float> stock_price_q42 = stock_price.array_query_no_materialize(x167, len166);
  Array<float> strike_price_q41 = strike_price.array_query_no_materialize(x167, len166);
  Array<float> volatility_q40 = volatility.array_query_no_materialize(x171, len170);
  Array<float> volatility_q39 = volatility.array_query_no_materialize(x181, len180);
  Array<float> stock_price_q38 = stock_price.array_query_no_materialize(x187, len186);
  Array<float> strike_price_q37 = strike_price.array_query_no_materialize(x193, len192);
  Array<float> out_q = out.array_query_no_materialize(x197, len196);
    mock::div(stock_price_q42.data, strike_price_q41.data, len166, s_over_k_q.data);
    mock::vlog(s_over_k_q.data, len168, log_res_q.data);
    mock::mul(volatility_q40.data, volatility_q40.data, len170, vol_sq_q.data);
    mock::divi(vol_sq_q.data, 2, len172, vol_sq_half_q.data);
    mock::addi(vol_sq_half_q.data, 0.01, len174, vol_sq_half_plusr_q.data);
    mock::muli(vol_sq_half_plusr_q.data, 0.25, len176, vol_inter_q.data);
    mock::add(log_res_q.data, vol_inter_q.data, len178, d1_inter_q.data);
    mock::muli(volatility_q39.data, 0.5, len180, vol_sqrt_T_q.data);
    mock::div(d1_inter_q.data, vol_sqrt_T_q.data, len182, d1_q.data);
    mock::cdf(d1_q.data, len184, d1_cdf_q.data);
    mock::mul(d1_cdf_q.data, stock_price_q38.data, len186, part1_q.data);
    mock::sub(d1_q.data, vol_sqrt_T_q.data, len188, d2_q.data);
    mock::cdf(d2_q.data, len190, d2_cdf_q.data);
    mock::muli(strike_price_q37.data, 0.997503, len192, k_scaled_q.data);
    mock::mul(d2_cdf_q.data, k_scaled_q.data, len194, part2_q.data);
    mock::sub(part1_q.data, part2_q.data, len196, out_q.data);
}

  s_over_k_q.destroy();
  log_res_q.destroy();
  vol_sq_q.destroy();
  vol_sq_half_q.destroy();
  vol_sq_half_plusr_q.destroy();
  vol_inter_q.destroy();
  d1_inter_q.destroy();
  vol_sqrt_T_q.destroy();
  d1_q.destroy();
  d1_cdf_q.destroy();
  part1_q.destroy();
  d2_q.destroy();
  d2_cdf_q.destroy();
  k_scaled_q.destroy();
  part2_q.destroy();
}
