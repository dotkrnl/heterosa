/*
 * This code implements the Matricized Tensor Times Khatri-Rao Product (MTTKRP),
 * which performs: D(i,j) += A(i,k,l) * B(k,j) * C(l,j) Input: A[I][K][L],
 * B[K][J], C[L][J] Output: D[I][J]
 */

#include "kernel.h"

int main() {
  // declarations
  static data_t A[I][K][L];
  static data_t B[K][J];
  static data_t C[J][L];
  static data_t D[I][J];
  static data_t D_golden[I][J];

  // data initialization
  for (int i = 0; i < I; i++)
    for (int k = 0; k < K; k++)
      for (int l = 0; l < L; l++) {
        A[i][k][l] = (data_t)rand() / RAND_MAX;
      }
  for (int k = 0; k < K; k++)
    for (int j = 0; j < J; j++) {
      B[k][j] = (data_t)rand() / RAND_MAX;
    }
  for (int l = 0; l < L; l++)
    for (int j = 0; j < J; j++) {
      C[j][l] = (data_t)rand() / RAND_MAX;
    }

    // computation
#pragma scop
  for (int i = 0; i < I; i++)
    for (int j = 0; j < J; j++) {
      D[i][j] = 0;
      for (int k = 0; k < K; k++) {
        for (int l = 0; l < L; l++) {
          D[i][j] = D[i][j] + A[i][k][l] * B[k][j] * C[j][l];
        }
      }
    }
#pragma endscop

  for (int i = 0; i < I; i++)
    for (int j = 0; j < J; j++) {
      D_golden[i][j] = 0;
      for (int k = 0; k < K; k++) {
        data_t tmp = 0;
        for (int l = 0; l < L; l++) {
          tmp += A[i][k][l] * C[j][l];
        }
        D_golden[i][j] += B[k][j] * tmp;
      }
    }

  // comparison
  int err = 0;
  float thres = 0.001;
  for (int i = 0; i < I; i++)
    for (int j = 0; j < J; j++) {
      if (fabs(((float)D_golden[i][j] - (float)D[i][j]) / (float)D[i][j]) >
          thres) {
        err++;
        printf("D[%d][%d] = %f, D_golden[%d][%d] = %f\n", i, j, D[i][j], i, j,
               D_golden[i][j]);
        if (err > 16) {
          printf("Too many errors, aborting\n");
          return -1;
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
