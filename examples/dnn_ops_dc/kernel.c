// In this example, we compile three different operators that are found often in
// DNNs, including: point-wise conv, depth-wise conv, and FC.

#include "kernel.h"

int main(int argc, char **argv) {
  // Depth-wise CONV
  data_t dc_cin[DC_R + DC_K - 1][DC_C + DC_K - 1][DC_I];
  data_t dc_w[DC_K][DC_K][DC_I];
  data_t dc_cout[DC_R][DC_C][DC_O];
  data_t dc_cout_golden[DC_R][DC_C][DC_O];

  for (int i = 0; i < DC_I; i++)
    for (int r = 0; r < DC_R + DC_K - 1; r++)
      for (int c = 0; c < DC_C + DC_K - 1; c++) {
        dc_cin[r][c][i] = i;
      }

  for (int i = 0; i < DC_I; i++)
    for (int p = 0; p < DC_K; p++)
      for (int q = 0; q < DC_K; q++) {
        dc_w[p][q][i] = i;
      }

#pragma scop
  for (int o = 0; o < DC_O; o++)
    for (int r = 0; r < DC_R; r++)
      for (int c = 0; c < DC_C; c++) {
        dc_cout[r][c][o] = 0;
        for (int p = 0; p < DC_K; p++)
          for (int q = 0; q < DC_K; q++) {
            dc_cout[r][c][o] =
                dc_cout[r][c][o] + dc_cin[r + p][c + q][o] * dc_w[p][q][o];
          }
      }
#pragma endscop

  for (int o = 0; o < DC_O; o++)
    for (int r = 0; r < DC_R; r++)
      for (int c = 0; c < DC_C; c++) {
        dc_cout_golden[r][c][o] = 0;
        for (int p = 0; p < DC_K; p++)
          for (int q = 0; q < DC_K; q++) {
            dc_cout_golden[r][c][o] = dc_cout_golden[r][c][o] +
                                      dc_cin[r + p][c + q][o] * dc_w[p][q][o];
          }
      }

  int err = 0;
  float thres = 0.001;
  for (int o = 0; o < DC_O; o++)
    for (int r = 0; r < DC_R; r++)
      for (int c = 0; c < DC_C; c++) {
        if (fabs((float)dc_cout_golden[r][c][o] - (float)dc_cout[r][c][o]) >
            thres) {
          err++;
          printf("(golden, hw)@(%d, %d, %d): (%f, %f)\n", o, r, c,
                 (float)dc_cout_golden[r][c][o], (float)dc_cout[r][c][o]);
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