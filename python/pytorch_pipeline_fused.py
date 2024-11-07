def my_fused_impl(x, y, z, matmul_1, var00, i_len7, var18, j_len9):
	for j6 in range(0, matmul_1.size(dim=1), j_len9):
		for i5 in range(0, matmul_1.size(dim=0), i_len7):
			
			i1 = i5
			j2 = 0
			i_len3 = i_len7
			j_len4 = var18
			matmul_q = Tensor.empty(i1, j2, i_len3, j_len4)
			y_q4 = y.matrix_query(0, j2, var00, j_len4)
			x_q3 = x.matrix_query(i1, 0, i_len3, var00)
			matmul_1_q = matmul_1.matrix_query(i5, j6, i_len7, j_len9)
			z_q2 = z.matrix_query(0, j6, var18, j_len9)
			Tensor.matmul(x_q3, y_q4, var00, matmul_q)
			Tensor.matmul(matmul_q, z_q2, var18, matmul_1_q)
			matmul_1.matrix_insert(i5, j6, i_len7, j_len9, matmul_1_q)

