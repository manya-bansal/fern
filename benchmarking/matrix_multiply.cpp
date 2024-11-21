#include "tensor.h"
#include <chrono>
#include <fstream>
// #include <matplot/matplot.h>
// using namespace matplot;

using namespace mock;
using namespace std;

inline __attribute__((always_inline)) void my_fused_impl(const Tensor<float> x, const Tensor<float> y, const Tensor<float> z, Tensor<float> matmul_1, int64_t var1351, int64_t var1557, int64_t i_len60, int64_t j_len61){
  int64_t j59 = 0;
 int64_t i58 = 0;
 int64_t i47 = i58;
 int64_t j48 = 0;
 int64_t i_len49 = i_len60;
 int64_t j_len50 = var1557;
 Tensor<float> matmul_q = Tensor<float>(i47, j48, i_len49, j_len50);
 for(int64_t j59 = 0;j59 < matmul_1.width; j59+=j_len61){
  for(int64_t i58 = 0;i58 < matmul_1.height; i58+=i_len60){
      int64_t i47 = i58;
   int64_t j48 = 0;
   int64_t i_len49 = i_len60;
   int64_t j_len50 = var1557;
   Tensor<float> y_q21 = y.matrix_query(0, j48, var1351, j_len50);
   Tensor<float> x_q20 = x.matrix_query(i47, 0, i_len49, var1351);
   Tensor<float> matmul_1_q = matmul_1.matrix_query(i58, j59, i_len60, j_len61);
   Tensor<float> z_q19 = z.matrix_query(0, j59, var1557, j_len61);
      matmul(x_q20, y_q21, matmul_q);
      matmul(matmul_q, z_q19, matmul_1_q);
      matmul_1.matrix_insert(i58, j59, i_len60, j_len61, matmul_1_q);
}
}

}



int main()
{
	ofstream naive_output("./matmul_data/hand");
	ofstream fused_output("./matmul_data/fused");
	// int MAT_SIZE = 2048;
	int MAT_SIZE = 1024;
	int NUM_TRIALS = 10;
	srand(time(0));
	vector<int> tile_sizes;
	vector<float> naive_times;
	vector<float> fused_times;
	for (int tile_sz = 16; tile_sz < MAT_SIZE; tile_sz *= 2) {
		tile_sizes.push_back(tile_sz);
		int naive_sum = 0;
		int fused_sum = 0;
		for (int trial = 0; trial < NUM_TRIALS; trial++) {
			cout << trial << endl;
			Tensor<float> a = Tensor<float>(0, 0, MAT_SIZE, MAT_SIZE);
			a.init_rand();

			Tensor<float> b = Tensor<float>(0, 0, MAT_SIZE, MAT_SIZE);
			b.init_rand();
			
			Tensor<float> c = Tensor<float>(0, 0, MAT_SIZE, MAT_SIZE);
			c.init_rand();
			
			Tensor<float> out1 = Tensor<float>(0, 0, MAT_SIZE, MAT_SIZE);
			Tensor<float> out = Tensor<float>(0, 0, MAT_SIZE, MAT_SIZE);

			auto t1 = chrono::high_resolution_clock::now();
			matmul(a, b, out1);
			matmul(out1, c, out);
			auto t2 = chrono::high_resolution_clock::now();
			auto ms = chrono::duration_cast<chrono::milliseconds>(t2 - t1);
			naive_sum += ms.count();
			
			Tensor<float> fused_out = Tensor<float>(0, 0, MAT_SIZE, MAT_SIZE);
			t1 = chrono::high_resolution_clock::now();
			my_fused_impl(a, b, c, fused_out, MAT_SIZE, MAT_SIZE, tile_sz, MAT_SIZE);
			t2 = chrono::high_resolution_clock::now();
			ms = chrono::duration_cast<chrono::milliseconds>(t2 - t1);
			fused_sum += ms.count();

			for (int i = 0; i < out.height; i++) {
				for (int j = 0; j < out.width; j++) {
					if (out.get(i, j) != fused_out.get(i, j)) {
						cout << "MISMATCH FOUND AT i = " << i << ", j = " << j << endl; 
						cout << "out(i, j): " << out.get(i, j) << endl;
						cout << "fused_out(i, j):" << fused_out.get(i, j) << endl;
					}
				}
			}
		}
		cout << "tile size " << tile_sz << endl;
		float naive_avg = (float)naive_sum / NUM_TRIALS;
		cout << "naive time avg " << naive_avg << endl;
		naive_output << tile_sz << endl;
		naive_output << naive_avg << endl;

		float fused_avg = (float)fused_sum / NUM_TRIALS;
		cout << "fused time avg " << fused_avg << endl;
		fused_output << tile_sz << endl;
		fused_output << fused_avg << endl;
		
	}
	naive_output.close();
	fused_output.close();
	// plot(tile_sizes, naive_times, "-o");
	// plot(tile_sizes, fused_times, "-ro");
	// show();
	
}