def my_fused_impl(a, b, out, len1):
	for x0 in range(out.idx, out.idx + out.size, len1):
		
		out_q = out.array_query(x0, len1)
		b_q1 = b.array_query(x0, len1)
		a_q0 = a.array_query(x0, len1)
		vadd(a_q0.data, b_q1.data, out_q.data)
		out.array_insert(x0, len1, out_q)

