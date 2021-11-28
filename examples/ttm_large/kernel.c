/*
 * This code implements the Tensor Times Matrix (TTM), which performs:
 * C(i,j,k) += A(i,j,l) * B(l,k)
 * Input: A[I][J][L], B[L][K]
 * Output: C[I][J][K]
 */

#include "kernel.h"

int main() {
  // declarations
  static data_t A[I][J][L];
  static data_t B[K][L];
  static data_t C[I][J][K];
  static data_t C_golden[I][J][K];

  // data initialization
  for (int i = 0; i < I; i++)
    for (int j = 0; j < J; j++)
      for (int l = 0; l < L; l++) {
        A[i][j][l] = (data_t)rand() / RAND_MAX;
      }
  for (int l = 0; l < L; l++)
    for (int k = 0; k < K; k++) {
      B[k][l] = (data_t)rand() / RAND_MAX;
    }

    // computation
#pragma scop
  for (int i = 0; i < I; i++)
    for (int j = 0; j < J; j++)
      for (int k = 0; k < K; k++) {
        C[i][j][k] = 0;
        for (int l = 0; l < L; l++) {
          C[i][j][k] = C[i][j][k] + A[i][j][l] * B[k][l];
        }
      }
#pragma endscop

  for (int i = 0; i < I; i++)
    for (int j = 0; j < J; j++)
      for (int k = 0; k < K; k++) {
        C_golden[i][j][k] = 0;
        for (int l = 0; l < L; l++) {
          C_golden[i][j][k] += A[i][j][l] * B[k][l];
        }
      }

  // comparison
  int err = 0;
  float thres = 0.001;
  for (int i = 0; i < I; i++)
    for (int j = 0; j < J; j++)
      for (int k = 0; k < K; k++) {
        if (fabs((C_golden[i][j][k] - C[i][j][k]) / C[i][j][k]) > thres) {
          err++;
          printf("C[%d][%d][%d] = %f, C_golden[%d][%d][%d] = %f\n", i, j, k,
                 C[i][j][k], i, j, k, C_golden[i][j][k]);
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
