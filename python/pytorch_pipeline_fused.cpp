void my_fused_impl(const Tensor<float> x, const Tensor<float> y, const Tensor<float> z, Tensor<float> matmul_1, int64_t var00, int64_t i_len7, int64_t var18, int64_t j_len9){
 for(int64_t j6 = 0;j6 < matmul_1.size(dim=1); j6+=j_len9){
  for(int64_t i5 = 0;i5 < matmul_1.size(dim=0); i5+=i_len7){
      int64_t i1 = i5;
   int64_t j2 = 0;
   int64_t i_len3 = i_len7;
   int64_t j_len4 = var18;
   Tensor<float> matmul_q = Tensor.empty(i1, j2, i_len3, j_len4);
   Tensor<float> y_q4 = y.matrix_query(0, j2, var00, j_len4);
   Tensor<float> x_q3 = x.matrix_query(i1, 0, i_len3, var00);
   Tensor<float> matmul_1_q = matmul_1.matrix_query(i5, j6, i_len7, j_len9);
   Tensor<float> z_q2 = z.matrix_query(0, j6, var18, j_len9);
      Tensor.matmul(x_q3, y_q4, var00, matmul_q);
      Tensor.matmul(matmul_q, z_q2, var18, matmul_1_q);
      matmul_1.matrix_insert(i5, j6, i_len7, j_len9, matmul_1_q);
}
}
}

