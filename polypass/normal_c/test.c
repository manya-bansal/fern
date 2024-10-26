#include <stdio.h>

void example(int N, int M, int arr[N][M]) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            arr[i][j] += i * j;
        }
    }
}