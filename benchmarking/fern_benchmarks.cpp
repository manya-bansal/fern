#include <benchmark/benchmark.h>
#include <string.h>
#include "tensor.h"
#include <math.h>
using namespace mock;
using namespace std;

__attribute__((noinline)) void mmul_unfused(const Tensor<float> a, const Tensor<float> b, Tensor<float> out) {
	matmul(a, b, out);
}

__attribute__((noinline)) void mmul_fused(const Tensor<float> a, const Tensor<float> b, Tensor<float> out, int64_t step_i, int64_t step_j) {
	Tensor<float> out_q = Tensor<float>(0, 0, step_i, step_j);
	for (int i = 0; i < out.height; i += step_i) {
		Tensor<float> a_q = a.matrix_query(i, 0, step_i, a.width);
		for (int j = 0; j < out.width; j += step_j) {
			Tensor<float> b_q = b.matrix_query(0, j, b.height, step_j);
			matmul(a_q, b_q, out_q);
			out.matrix_insert(i, j, step_i, step_j, out_q);
		}
	}
}

#define MATMUL_ARRAY_SZ 1024

static void BM_UnfusedMatmul(benchmark::State& state) {
    Tensor<float> a = Tensor<float>(0, 0, MATMUL_ARRAY_SZ, MATMUL_ARRAY_SZ);
    Tensor<float> b = Tensor<float>(0, 0, MATMUL_ARRAY_SZ, MATMUL_ARRAY_SZ);
    a.init_rand();
    b.init_rand();
    Tensor<float> out = Tensor<float>(0, 0, MATMUL_ARRAY_SZ, MATMUL_ARRAY_SZ);
    for (auto _ : state) {
        mmul_unfused(a, b, out);
    }
}

static void BM_FusedMatmul(benchmark::State& state) {
    Tensor<float> a = Tensor<float>(0, 0, MATMUL_ARRAY_SZ, MATMUL_ARRAY_SZ);
    Tensor<float> b = Tensor<float>(0, 0, MATMUL_ARRAY_SZ, MATMUL_ARRAY_SZ);
    a.init_rand();
    b.init_rand();
    Tensor<float> out = Tensor<float>(0, 0, MATMUL_ARRAY_SZ, MATMUL_ARRAY_SZ);
    for (auto _ : state) {
        mmul_fused(a, b, out, state.range(0), state.range(0));
    }
}

__attribute__((noinline)) void attention_my_fused_impl(const Tensor<float> l_k_, const Tensor<float> l_q_, const Tensor<float> l_v_, float _sym_sqrt, Tensor<float> matmul_1, int64_t shared_len17, int64_t j_len40, int64_t i_len39, int64_t shared_len41){
  int64_t j38 = 0;
 int64_t i37 = 0;
 int64_t i29 = i37;
 int64_t i_len30 = i_len39;
 int64_t width31 = shared_len41;
 int64_t i22 = i29;
 int64_t j23 = 0;
 int64_t i_len24 = i_len30;
 int64_t j_len25 = width31;
 int64_t i13 = i22;
 int64_t j14 = j23;
 int64_t i_len15 = i_len24;
 int64_t j_len16 = j_len25;
 int64_t i4 = 0;
 int64_t j7 = j14;
 int64_t i_len5 = shared_len17;
 int64_t j_len6 = j_len16;
 Tensor<float> t_q = Tensor<float>(i4, j7, i_len5, j_len6);
 Tensor<float> matmul_q = Tensor<float>(i13, j14, i_len15, j_len16);
 Tensor<float> truediv_q = Tensor<float>(i22, j23, i_len24, j_len25);
 Tensor<float> softmax_q = Tensor<float>(i29, 0, i_len30, width31);
 for(int64_t j38 = 0;j38 < matmul_1.width; j38+=j_len40){
  for(int64_t i37 = 0;i37 < matmul_1.height; i37+=i_len39){
      int64_t i29 = i37;
   int64_t i_len30 = i_len39;
   int64_t width31 = shared_len41;
   int64_t i22 = i29;
   int64_t j23 = 0;
   int64_t i_len24 = i_len30;
   int64_t j_len25 = width31;
   int64_t i13 = i22;
   int64_t j14 = j23;
   int64_t i_len15 = i_len24;
   int64_t j_len16 = j_len25;
   int64_t i4 = 0;
   int64_t j7 = j14;
   int64_t i_len5 = shared_len17;
   int64_t j_len6 = j_len16;
   Tensor<float> l_k__q5 = l_k_.matrix_query(j7, i4, j_len6, i_len5);
   Tensor<float> l_q__q4 = l_q_.matrix_query(i13, 0, i_len15, shared_len17);
   Tensor<float> matmul_1_q = matmul_1.matrix_query(i37, j38, i_len39, j_len40);
   Tensor<float> l_v__q3 = l_v_.matrix_query(0, j38, shared_len41, j_len40);
      transpose(l_k__q5, t_q);
      matmul(l_q__q4, t_q, matmul_q);
      divn(matmul_q, _sym_sqrt, truediv_q);
      softmax(truediv_q, softmax_q);
      matmul(softmax_q, l_v__q3, matmul_1_q);
      matmul_1.matrix_insert(i37, j38, i_len39, j_len40, matmul_1_q);
}
}

}

__attribute__((noinline)) void attn_unfused(const Tensor<float> q, const Tensor<float> k, const Tensor<float> v, float sqrt_dk, Tensor<float> out) {
	Tensor<float> kt = Tensor<float>(0, 0, k.width, k.height);
	transpose(k, kt);
	Tensor<float> q_kt = Tensor<float>(0, 0, q.height, kt.width);
	Tensor<float> softmax_in = Tensor<float>(0, 0, q.height, kt.width);
	Tensor<float> softmax_out = Tensor<float>(0, 0, q.height, kt.width);
	matmul(q, kt, q_kt);
	divn(q_kt, sqrt_dk, softmax_in);
	softmax(softmax_in, softmax_out);
	matmul(softmax_out, v, out);
}


#define nk 1024
#define dk 256

static void BM_Attention_Unfused(benchmark::State& state) {
    Tensor<float> q = Tensor<float>(0, 0, nk, dk);
    Tensor<float> k = Tensor<float>(0, 0, nk, dk);
    Tensor<float> v = Tensor<float>(0, 0, nk, dk);
    Tensor<float> out = Tensor<float>(0, 0, nk, dk);
    q.init_rand();
    k.init_rand();
    v.init_rand();
    for (auto _ : state) {
        attn_unfused(q, k, v, sqrt(q.width), out);
    }
}

static void BM_Attention_Fused(benchmark::State& state) {
    Tensor<float> q = Tensor<float>(0, 0, nk, dk);
    Tensor<float> k = Tensor<float>(0, 0, nk, dk);
    Tensor<float> v = Tensor<float>(0, 0, nk, dk);
    Tensor<float> out = Tensor<float>(0, 0, nk, dk);
    q.init_rand();
    k.init_rand();
    v.init_rand();
    for (auto _ : state) {
        attention_my_fused_impl(k, q, v, sqrt(q.width), out, q.width, q.width, state.range(0), v.height);
    }
}


// BENCHMARK(BM_UnfusedMatmul)->MinTime(10)->MinWarmUpTime(10)->Iterations(10)->Repetitions(5);
// BENCHMARK(BM_FusedMatmul)->MinTime(10)->MinWarmUpTime(10)->Iterations(10)->Repetitions(5)->RangeMultiplier(2)->Range(16, MATMUL_ARRAY_SZ);
BENCHMARK(BM_Attention_Unfused)->MinTime(10)->MinWarmUpTime(10)->Iterations(10)->Repetitions(5);
BENCHMARK(BM_Attention_Fused)->MinTime(10)->MinWarmUpTime(10)->Iterations(10)->Repetitions(5)->RangeMultiplier(2)->Range(16, nk);


BENCHMARK_MAIN();