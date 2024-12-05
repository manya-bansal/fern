def my_fused_impl(l_k_, l_q_, l_v_, _sym_sqrt, matmul_1, i_len32, shared_len34, j_len35, shared_len12):
	
	j33 = 0
	i36 = 0
	i26 = i36
	i_len27 = i_len32
	width28 = shared_len34
	i18 = i26
	j19 = 0
	i_len20 = i_len27
	j_len21 = width28
	i8 = i18
	j9 = j19
	i_len10 = i_len20
	j_len11 = j_len21
	i0 = 0
	j1 = j9
	i_len2 = shared_len12
	j_len3 = j_len11
	t_q = Tensor.empty(i0, j1, i_len2, j_len3)
	matmul_q = Tensor.empty(i8, j9, i_len10, j_len11)
	truediv_q = Tensor.empty(i18, j19, i_len20, j_len21)
	softmax_q = Tensor.empty(i26, 0, i_len27, width28)
	for j33 in range(0, matmul_1.size(dim=1), j_len35):
		for i36 in range(0, matmul_1.size(dim=0), i_len32):
			
			i26 = i36
			i_len27 = i_len32
			width28 = shared_len34
			i18 = i26
			j19 = 0
			i_len20 = i_len27
			j_len21 = width28
			i8 = i18
			j9 = j19
			i_len10 = i_len20
			j_len11 = j_len21
			i0 = 0
			j1 = j9
			i_len2 = shared_len12
			j_len3 = j_len11
			l_k__q2 = l_k_.matrix_query(j1, i0, j_len3, i_len2)
			l_q__q1 = l_q_.matrix_query(i8, 0, i_len10, shared_len12)
			matmul_1_q = matmul_1.matrix_query(i36, j33, i_len32, j_len35)
			l_v__q0 = l_v_.matrix_query(0, j33, shared_len34, j_len35)
			Tensor.transpose(l_k__q2, t_q)
			Tensor.matmul(l_q__q1, t_q, matmul_q)
			Tensor.divn(matmul_q, _sym_sqrt, truediv_q)
			Tensor.softmax(truediv_q, softmax_q)
			Tensor.matmul(softmax_q, l_v__q0, matmul_1_q)
			matmul_1.matrix_insert(i36, j33, i_len32, j_len35, matmul_1_q)


