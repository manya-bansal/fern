#include "tensor.h"
#include <vector>
#include <chrono>
#include <fstream>
using namespace mock;
using namespace std;

inline __attribute__((always_inline)) void my_fused_impl(const Tensor<float> l_k_, const Tensor<float> l_q_, const Tensor<float> l_v_, float _sym_sqrt, Tensor<float> matmul_1, int64_t shared_len17, int64_t j_len40, int64_t i_len39, int64_t shared_len41){
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
//  Tensor<float> truediv_q = Tensor<float>(i22, j23, i_len24, j_len25);
//  Tensor<float> softmax_q = Tensor<float>(i29, 0, i_len30, width31);
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
      divn(matmul_q, _sym_sqrt, matmul_q);
      softmax(matmul_q, matmul_q);
      matmul(matmul_q, l_v__q3, matmul_1_q);
      matmul_1.matrix_insert(i37, j38, i_len39, j_len40, matmul_1_q);
}
}

}




inline __attribute__((always_inline)) void attn_unfused(const Tensor<float> q, const Tensor<float> k, const Tensor<float> v, float sqrt_dk, Tensor<float> out) {
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


const float EPSILON = 1e-5;
#define NUM_TRIALS 50
#define TRIALS_COUNTED 40

int main()
{
	ofstream naive_output("./attention_data/hand");
	ofstream fused_output("./attention_data/fused");

	int nk = 2048;
	int dk = 256;

	for (int tile_sz = 32; tile_sz <= 640; tile_sz += 64) {
		int naive_sum = 0;
		int fused_sum = 0;
		for (int trial = 0; trial < NUM_TRIALS; trial++) {
			// cout << trial << endl;
			Tensor<float> q = Tensor<float>(0, 0, nk, dk);
			Tensor<float> k = Tensor<float>(0, 0, nk, dk);
			Tensor<float> v = Tensor<float>(0, 0, nk, dk);
			Tensor<float> out = Tensor<float>(0, 0, nk, dk);
			q.init_rand();
			k.init_rand();
			v.init_rand();

			auto t1 = chrono::high_resolution_clock::now();
			attn_unfused(q, k, v, sqrt(q.width), out);
			auto t2 = chrono::high_resolution_clock::now();
			auto ms = chrono::duration_cast<chrono::milliseconds>(t2 - t1);
			cout << "naive time: " << ms.count() << " ";
			if (trial >= NUM_TRIALS - TRIALS_COUNTED) {
				naive_sum += ms.count();
			}

			Tensor<float> fused_out = Tensor<float>(0, 0, nk, dk);
			// attn_fused(k, q, v, fused_out, q.width, v.height, 1, q.width);
			t1 = chrono::high_resolution_clock::now();
			my_fused_impl(k, q, v, sqrt(q.width), fused_out, q.width, q.width, tile_sz, v.height);
			t2 = chrono::high_resolution_clock::now();
			ms = chrono::duration_cast<chrono::milliseconds>(t2 - t1);
			cout << "fused time: " << ms.count() << endl;
			if (trial >= NUM_TRIALS - TRIALS_COUNTED) {
				fused_sum += ms.count();
			}
			// cout << fused_out << endl;
			for (int i = 0; i < out.height; i++) {
				for (int j = 0; j < out.width; j++) {
					if (abs(fused_out.get(i, j) - out.get(i, j)) > EPSILON) {
						cout << "MISMATCH at (" << i << ", " << j << "): expected " << out.get(i, j) << " got " << fused_out.get(i, j) << endl;
					}
				}
			}
		}

		cout << "tile size " << tile_sz << endl;
		double naive_avg = (double)naive_sum / TRIALS_COUNTED;
		cout << "naive time avg " << naive_avg << endl;
		naive_output << tile_sz << endl;
		naive_output << naive_avg << endl;

		double fused_avg = (double)fused_sum / TRIALS_COUNTED;
		cout << "fused time avg " << fused_avg << endl;
		fused_output << tile_sz << endl;
		fused_output << fused_avg << endl;
	}
	naive_output.close();
	fused_output.close();
	
}