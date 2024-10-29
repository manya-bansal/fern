class Array:
    def __init__(self, idx, length):
        self.data = [0 for i in range(0, length)]
        self.size = length
        self.idx = idx  # Starting index for querying
    
    def array_query(self, start_idx, length):
        a =  Array(start_idx, length)
        new_data = [self.data[start_idx+ i] for i in range(length)]
        a.set_data(new_data)
        return a

    def size(self):
        return self.size

    def idx(self):
        return self.idx
    
    def set_data(self, data):
        self.data = data

    def array_insert(self, idx, length, array):
        for i in range(0, length):
           self.data[i + idx] = array.data[i]

def vadd(src1, src2, target):
    # Vectorized addition: element-wise addition of src1 and src2, storing in target
    for i in range(len(target.data)):
        target.data[i] = src1.data[i] + src2.data[i]

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

length = 8

a_data = [i for i in range(length)]
b_data = [i for i in range(length)]
c_data = [i for i in range(length)]
result_data = [0 for i in range(length)]

a = Array(0, len(a_data))
b = Array(0, len(b_data))
c = Array(0, len(c_data))
result = Array(0, len(result_data))

a.set_data(a_data)
b.set_data(b_data)
c.set_data(c_data)
result.set_data(result_data)


my_fused_impl(a, b, c, result, 4)

print(result.data)