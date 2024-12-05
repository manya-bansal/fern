#ifndef MY_TENSOR_INTERFACE
#define MY_TENSOR_INTERFACE
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <Accelerate/Accelerate.h>
// #include <cassert>

namespace mock {

template <typename T> class Tensor {
public:
  Tensor() = default;
  Tensor (std::initializer_list<std::initializer_list<T>> l) {
    height = l.size();
    logical_height = height;
    width = (*l.begin()).size();
    logical_width = width;
    int idx = 0;
    for (auto row : l) {
        std::copy(row.begin(), row.end(), data + idx * width);
        idx++;
    }
  }

  explicit Tensor(int i, int j, int i_len, int j_len) : i_idx(i), j_idx(j), height(i_len), logical_height(i_len), width(j_len), logical_width(j_len) {
    data = (T *)malloc(sizeof(T) * i_len * j_len);
  }
  void empty(int i, int j, int i_len, int j_len) {
    i_idx = i;
    j_idx = j;
    height = i_len;
    logical_height = i_len;
    width = j_len;
    logical_width = j_len;
    data = (T *)malloc(sizeof(T) * i_len * j_len);
  }

  void init_rand() {
    for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			set(i, j, (T)(rand() % 2500) / 1000);
		}
	}
  }


  void init_rand_int() {
    for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			set_int(i, j, rand() % 100);
		}
	}
  }

  inline __attribute__((always_inline)) T get(int i, int j) const {
    return data[(i_idx + i) * logical_width + (j_idx + j)];
  }

  inline __attribute__((always_inline)) void set(int i, int j, T val) {
    data[(i_idx + i) * logical_width + (j_idx + j)] = val;
  }

  void set_int(int i, int j, int val) {
    data[(i_idx + i) * logical_width + (j_idx + j)] = val;
  }

  inline Tensor<T> matrix_query(int i, int j, int i_len, int j_len) const {
    Tensor<T> a;
    a.i_idx = i;
    a.j_idx = j;
    a.height = std::min(i_len, i_idx + height - i);
    a.width = std::min(j_len, j_idx + width - j);
    a.data = data;
    a.logical_height = logical_height;
    a.logical_width = logical_width;
    return a;
  }

  inline void matrix_insert(int i_start, int j_start, int i_len, int j_len, Tensor<T> tensor) {
    for (int i = 0; i < i_len; i++) {
        for (int j = 0; j < j_len; j++) {
            set(i_start + i, j_start + j, tensor.get(i, j));
        }
    }
  }

  T *data;
  int i_idx = 0;
  int j_idx = 0;
  int height;
  int width;
  int logical_height;
  int logical_width;
};
template <typename T>
std::ostream &operator<<(std::ostream &os, Tensor<T> const &a) {
  std::cout << a.height << "x" << a.width;
  std::cout << " {" << std::endl;
  for (int i = 0; i < a.height; i++) {
    std::cout << "{ ";
    for (int j = 0; j < a.width - 1; j++) {
        std::cout << a.get(i, j) << ", ";
    }
    std::cout << a.get(i, a.width - 1);
    std::cout << " }" << std::endl;
  }
  std:: cout << "}" << std::endl;
  
  return os;
}

template <typename T> __attribute__((noinline)) void matmul(Tensor<T> a, Tensor<T> b, Tensor<T> out) {
	cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, a.height, b.width, a.width, 1, a.data + a.i_idx * a.logical_width + a.j_idx, a.logical_width, b.data + b.i_idx * b.logical_width + b.j_idx, b.logical_width, 1, out.data + out.i_idx * out.logical_width + out.j_idx, out.logical_width);
    // for (int i = 0; i < a.height; i++) {
    //     for (int j = 0; j < b.width; j++) {
	// 		// std::cout << a.height << " " << b.width << std::endl;
    //         T acc = 0;
    //         for (int k = 0; k < a.width; k++) {
    //             acc += a.get(i, k) * b.get(k, j);
    //         }
    //         out.set(i, j, acc);
    //     }
    // }
}

template <typename T> __attribute__((noinline)) void softmax(Tensor<T> a, Tensor<T> out) {
    for (int i = 0; i < a.height; i++) {
        float exp_sum = 0;	
        for (int j = 0; j < a.width; j++) {
            exp_sum += a.get(i, j);
        }
        for (int j = 0; j < a.width; j++) {
            out.set(i, j, a.get(i, j) / exp_sum);
        }
    }
}

template <typename T> __attribute__((noinline)) void transpose(Tensor<T> a, Tensor<T> out) {
    for (int i = 0; i < a.height; i++) {
        for (int j = 0; j < a.width; j++) {
            out.set(j, i, a.get(i, j));
        }
    }
}

template <typename T> __attribute__((noinline)) void divn(Tensor<T> a, float n, Tensor<T> out) {
    for (int i = 0; i < a.height; i++) {
        for (int j = 0; j < a.width; j++) {
            out.set(i, j, a.get(i, j) / n);
        }
    }
}

template <typename T> void attention(Tensor<T> q, Tensor<T> k, Tensor<T> v, float d_k, Tensor<T> out) {
    // for (int i = 0; i < a.height; i++) {
    //     float exp_sum = 0;	
    //     for (int j = 0; j < a.width; j++) {
    //         exp_sum += exp(a.get(i, j));
    //     }
    //     for (int j = 0; j < a.width; j++) {
    //         out.set(i, j, exp(a.get(i, j)) / exp_sum);
    //     }
    // }
}


} // namespace mock

#endif