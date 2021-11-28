/*
 * This code implements the Chain of Tensor-matrix multiplications (TTMc), which
 * performs: D(i,j,k) += A(i,l,m) * B(l,j) * C(m,k) Input: A[I][L][M], B[L][J],
 * C[M][K] Output: D[I][J][K]
 */

#include "kernel.h"

int main() {
  // declarations
  static data_t A[I][M][L];
  static data_t B[J][L];
  static data_t C[K][M];
  static data_t D[I][J][K];
  static data_t D_golden[I][J][K];

  // data initialization
  for (int i = 0; i < I; i++)
    for (int l = 0; l < L; l++)
      for (int m = 0; m < M; m++) {
        A[i][m][l] = (data_t)rand() / RAND_MAX;
      }
  for (int l = 0; l < L; l++)
    for (int j = 0; j < J; j++) {
      B[j][l] = (data_t)rand() / RAND_MAX;
    }
  for (int m = 0; m < M; m++)
    for (int k = 0; k < K; k++) {
      C[k][m] = (data_t)rand() / RAND_MAX;
    }

    // computation
#pragma scop
  for (int i = 0; i < I; i++)
    for (int j = 0; j < J; j++)
      for (int k = 0; k < K; k++) {
        D[i][j][k] = 0;
        for (int l = 0; l < L; l++)
          for (int m = 0; m < M; m++) {
            D[i][j][k] = D[i][j][k] + A[i][m][l] * B[j][l] * C[k][m];
          }
      }
#pragma endscop

  for (int i = 0; i < I; i++)
    for (int j = 0; j < J; j++)
      for (int k = 0; k < K; k++) {
        D_golden[i][j][k] = 0;
        for (int l = 0; l < L; l++)
          for (int m = 0; m < M; m++) {
            D_golden[i][j][k] += A[i][m][l] * B[j][l] * C[k][m];
          }
      }

  // comparison
  int err = 0;
  float thres = 0.001;
  for (int i = 0; i < I; i++)
    for (int j = 0; j < J; j++)
      for (int k = 0; k < K; k++) {
        if (fabs((D_golden[i][j][k] - D[i][j][k]) / D[i][j][k]) > thres) {
          err++;
          printf("D[%d][%d][%d] = %f, D_golden[%d][%d][%d] = %f\n", i, j, k,
                 D[i][j][k], i, j, k, D_golden[i][j][k]);
          if (err > 10) {
            printf("Too many errors, exiting\n");
            exit(-1);
          }
        }
      }

  if (err) {
    printf("Test failed with %d errors!\n", err);
    return -1;
  } else {
    printf("Test passed!\n");
    return 0;
  }
}
