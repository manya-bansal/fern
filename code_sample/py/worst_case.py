def my_fused_impl(c, a, b, result, len252):
	
	x253 = 0
	x250 = x253
	len251 = len252
	x249 = x250
	len248 = len251
	x246 = x249
	len247 = len248
	x244 = x246
	len245 = len247
	add_1_q = Array(x244, len245)
	for x253 in range(result.idx, result.idx + result.size, len252):
		
		x250 = x253
		len251 = len252
		x249 = x250
		len248 = len251
		x246 = x249
		len247 = len248
		x244 = x246
		len245 = len247
		c_q53 = c.array_query(x244, len245)
		a_q52 = a.array_query(x246, len247)
		b_q51 = b.array_query(x249, len248)
		b_q50 = b.array_query(x250, len251)
		result_q = result.array_query(x253, len252)
		a_q49 = a.array_query(x253, len252)
		vadd(c_q53, c_q53, add_1_q)
		vadd(a_q52, add_1_q, add_1_q)
		vadd(b_q51, add_1_q, add_1_q)
		vadd(b_q50, add_1_q, add_1_q)
		vadd(a_q49, add_1_q, result_q)
		result.array_insert(x253, len252, result_q)


