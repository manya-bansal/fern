def my_fused_impl(c, a, b, result, len9):
	
	x8 = 0
	x6 = x8
	len7 = len9
	x4 = x6
	len5 = len7
	x2 = x4
	len3 = len5
	x0 = x2
	len1 = len3
	add_1_q = Array(x0, len1)
	for x8 in range(result.idx, result.idx + result.size, len9):
		
		x6 = x8
		len7 = len9
		x4 = x6
		len5 = len7
		x2 = x4
		len3 = len5
		x0 = x2
		len1 = len3
		c_q4 = c.array_query(x0, len1)
		a_q3 = a.array_query(x2, len3)
		b_q2 = b.array_query(x4, len5)
		b_q1 = b.array_query(x6, len7)
		result_q = result.array_query(x8, len9)
		a_q0 = a.array_query(x8, len9)
		vadd(c_q4, c_q4, add_1_q)
		vadd(a_q3, add_1_q, add_1_q)
		vadd(b_q2, add_1_q, add_1_q)
		vadd(b_q1, add_1_q, add_1_q)
		vadd(a_q0, add_1_q, result_q)
		result.array_insert(x8, len9, result_q)


