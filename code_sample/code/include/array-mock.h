#ifndef MY_ARRAY_INTERFACE
#define MY_ARRAY_INTERFACE
#include <stdlib.h>
#include <iostream>
#include <cmath>
// #include <cassert>

namespace mock {

template <typename T> class Array {
public:
  Array() = default;
  explicit Array(int size) : size(size), logical_size(size) {
	data = (T *)malloc(sizeof(T) * size);
  }
  void vvals(int val) {
	for (int i = 0; i < size; i++) {
	  data[i] = val;
	}
  }
  void destroy() {

  }
  inline Array<T> array_query_no_materialize(int index, int length) const {
	Array<T> a;
	a.idx = idx;
	a.data = data + idx;
	// for (int i = 0; i < length; i++) {
	//   a.data[i] = data[index + i];
	// }
	// a.idx = index;
	return a;
  }
  inline Array<T> array_query_materialize(int index, int length) {
	Array<T> a(length);
	for (int i = 0; i < length; i++) {
	  a.data[i] = data[index + i];
	}
	a.idx = index;
	return a;
  }
  inline void array_insert_materialize(int index, int length, Array<T> array) {
	for (int i = 0; i < length; i++) {
	  data[index + i] = array.data[i];
	}
  }
  void vals() {
	for (int i = 0; i < size; i++) {
	  data[i] = i;
	}
  }
  T *data;
  int size;
  int idx = 0;
  int logical_size;
};
template <typename T>
std::ostream &operator<<(std::ostream &os, Array<T> const &a) {
  std::cout << a.logical_size;
  std::cout << "{ ";
  for (int i = 0; i < a.logical_size - 1; i++) {
	std::cout << a.data[i] << ", ";
  }
  std::cout << a.data[a.logical_size - 1];
  std::cout << " }" << std::endl;
  return os;
}
template <typename T> inline Array<T> array_alloc(int index, int length) {
  Array<T> a(length);
  a.idx = index;
  return a;
}
template <typename T> void vadd(Array<T> a, Array<T> b, Array<T> out) {
  // assert(a.logical_size == b.logical_size);
  // assert(b.logical_size == out.logical_size);
  for (int i = 0; i < a.logical_size; i++) {
	out.data[i] = a.data[i] + b.data[i];
  }
  // std::cout << out << std::endl;
}

template <typename T> void addi(T *data, T b, int64_t len, T *out_data) {
	for (int i = 0; i < len; i++) {
		out_data[i] = data[i] + b;
	}
}

template <typename T> void subi(T *data, T b, int64_t len, T *out_data) {
	for (int i = 0; i < len; i++) {
		out_data[i] = data[i] - b;
	}
}

template <typename T> void muli(T *data, T b, int64_t len, T *out_data) {
	for (int i = 0; i < len; i++) {
		out_data[i] = data[i] * b;
	}
}

template <typename T> void divi(T *data, T b, int64_t len, T *out_data) {
	for (int i = 0; i < len; i++) {
		out_data[i] = data[i] / b;
	}
}

template <typename T> void add(T *a_data, T *b_data, int64_t len, T *out_data) {
	for (int i = 0; i < len; i++) {
		out_data[i] = a_data[i] + b_data[i];
	}
}

template <typename T> void sub(T *a_data, T *b_data, int64_t len, T *out_data) {
	for (int i = 0; i < len; i++) {
		out_data[i] = a_data[i] - b_data[i];
	}
}

template <typename T> void mul(T *a_data, T *b_data, int64_t len, T *out_data) {
	for (int i = 0; i < len; i++) {
		out_data[i] = a_data[i] * b_data[i];
	}
}

template <typename T> void div(T *a_data, T *b_data, int64_t len, T *out_data) {
	for (int i = 0; i < len; i++) {
		out_data[i] = a_data[i] / b_data[i];
	}
}

template <typename T> void cdf(T *data, int64_t len, T *out_data) {
	for (int i = 0; i < len; i++) {
		// https://en.cppreference.com/w/cpp/numeric/math/erfc
		out_data[i] = std::erfc(-data[i] / std::sqrt(2)) / 2;
	}
}

template <typename T> void vlog(T *data, int64_t len, T *out_data) {
	for (int i = 0; i < len; i++) {
		out_data[i] = log(data[i]);
	}
}

} // namespace mock

#endif