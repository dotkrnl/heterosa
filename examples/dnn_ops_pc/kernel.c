// In this example, we compile three different operators that are found often in
// DNNs, including: point-wise conv, depth-wise conv, and FC.

#include "kernel.h"

int main(int argc, char **argv) {
  // Point-wise CONV
  data_t pc_cin[PC_R + PC_K - 1][PC_C + PC_K - 1][PC_I];
  data_t pc_w[PC_O][PC_K][PC_K][PC_I];
  data_t pc_cout[PC_R][PC_C][PC_O];
  data_t pc_cout_golden[PC_R][PC_C][PC_O];

  for (int i = 0; i < PC_I; i++)
    for (int r = 0; r < PC_R + PC_K - 1; r++)
      for (int c = 0; c < PC_C + PC_K - 1; c++) {
        pc_cin[r][c][i] = i;
      }

  for (int o = 0; o < PC_O; o++)
    for (int i = 0; i < PC_I; i++)
      for (int p = 0; p < PC_K; p++)
        for (int q = 0; q < PC_K; q++) {
          pc_w[o][p][q][i] = o;
        }

#pragma scop
  for (int o = 0; o < PC_O; o++)
    for (int r = 0; r < PC_R; r++)
      for (int c = 0; c < PC_C; c++) {
        pc_cout[r][c][o] = 0;
        for (int i = 0; i < PC_I; i++)
          for (int p = 0; p < PC_K; p++)
            for (int q = 0; q < PC_K; q++) {
              pc_cout[r][c][o] =
                  pc_cout[r][c][o] + pc_cin[r + p][c + q][i] * pc_w[o][p][q][i];
            }
      }
#pragma endscop

  for (int o = 0; o < PC_O; o++)
    for (int r = 0; r < PC_R; r++)
      for (int c = 0; c < PC_C; c++) {
        pc_cout_golden[r][c][o] = 0;
        for (int i = 0; i < PC_I; i++)
          for (int p = 0; p < PC_K; p++)
            for (int q = 0; q < PC_K; q++) {
              pc_cout_golden[r][c][o] =
                  pc_cout_golden[r][c][o] +
                  pc_cin[r + p][c + q][i] * pc_w[o][p][q][i];
            }
      }

  int err = 0;
  float thres = 0.001;
  for (int o = 0; o < PC_O; o++)
    for (int r = 0; r < PC_R; r++)
      for (int c = 0; c < PC_C; c++) {
        if (fabs((float)pc_cout_golden[r][c][o] - (float)pc_cout[r][c][o]) >
            thres) {
          err++;
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