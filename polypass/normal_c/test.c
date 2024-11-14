// #include <stdio.h>

// void example4(int N, int M, int arr[N][M], int arr2[N][M]) {

//     for (int i = 0; i < N; ++i) {
//         for (int j = 0; j < M; ++j) {
//             // if (N%2 == 0){
//                 arr[i][j] =  arr2[i+1][j];
//             // }
//         }
//     }

//     for (int i = 0; i < N; ++i) {
//         for (int j = 0; j < (M); ++j) {
//             // M+1 doenst work in the one below.....
//             // for (int j = 0; j < (M); ++j) {
//             // if (N%2 == 0){
//                 // arr[i] =  arr2[i+1];
//             // }
//         }
//     }
// }

// void example2(int N, int arr[N], int arr2[N]) {
//     for (int i = 0; i < N; i++)
//         arr[i] =  arr2[i];
// }

void f(long A[], long N) {
  long i;
  long j;
  if (1)
    for (i = 0; i < N; ++i)
      A[i] = i;
  else
    for (j = 0; j < N; ++j)
      A[j] = j;
}

// typedef long BLASLONG;
// #define FLOAT	float

// int test(int n, int x[n], int y[n])
// {
// 	// BLASLONG i=0;
// 	// BLASLONG ix,iy;

// 	// if ( n < 0     )  return(0);
// 	// if ( da == 0.0 ) return(0);

// 	// ix = 0;
// 	// iy = 0;

//     for (int i = 0; i < n; i++){
//         x[i] = y[n];
//     }

// 	// // while(i < n)
// 	// // {

// 	// // 	y[iy] += da * x[ix] ;
// 	// // 	ix += inc_x ;
// 	// // 	iy += inc_y ;
// 	// // 	i++ ;

// 	// // }
// 	// return(0);
//     return 0;

// }
