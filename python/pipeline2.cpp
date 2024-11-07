void my_fused_impl(const Array<float> var0, Array<float> var4, int64_t len6){
 for(int64_t x7 = var4.idx;x7 < var4.idx + var4.size; x7+=len6){
    int64_t x5 = x7;
  int64_t len4 = len6;
  int64_t x3 = x5;
  int64_t len2 = len4;
  int64_t x0 = x5;
  int64_t len1 = len4;
  Array<float> var1_q = array_alloc<float>(x0, len1);
  Array<float> var0_q5 = var0.array_query_no_materialize(x0, len1);
  Array<float> var2_q = array_alloc<float>(x3, len2);
  Array<float> var3_q = array_alloc<float>(x5, len4);
  Array<float> var4_q = var4.array_query_no_materialize(x7, len6);
    mock::addi(var0_q5.data, 3
, len1, var1_q.data);
    mock::addi(var1_q.data, 2
, len2, var2_q.data);
    mock::add(var1_q.data, var2_q.data, len4, var3_q.data);
    mock::addi(var3_q.data, 1
, len6, var4_q.data);
    var1_q.destroy();
    var2_q.destroy();
    var3_q.destroy();
}
}

