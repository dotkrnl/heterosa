// In this example, we compile three different operators that are found often in
// DNNs, including: point-wise conv, depth-wise conv, and FC.

#include "kernel.h"

int main(int argc, char **argv) {
  // Fully-connected Layers
  data_t fc_cin[FC_I][FC_J];
  data_t fc_w[FC_J];
  data_t fc_cout[FC_I];
  data_t fc_cout_golden[FC_I];

  for (int i = 0; i < FC_I; i++)
    for (int j = 0; j < FC_J; j++) {
      fc_cin[i][j] = i;
    }

  for (int j = 0; j < FC_J; j++) {
    fc_w[j] = j;
  }

#pragma scop
  for (int i = 0; i < FC_I; i++) {
    fc_cout[i] = 0;
    for (int j = 0; j < FC_J; j++) {
      fc_cout[i] = fc_cout[i] + fc_cin[i][j] * fc_w[j];
    }
  }
#pragma endscop

  for (int i = 0; i < FC_I; i++) {
    fc_cout_golden[i] = 0;
    for (int j = 0; j < FC_J; j++) {
      fc_cout_golden[i] = fc_cout_golden[i] + fc_cin[i][j] * fc_w[j];
    }
  }

  int err = 0;
  float thres = 0.001;
  for (int i = 0; i < FC_I; i++)
    if (fabs((float)fc_cout_golden[i] - (float)fc_cout[i]) > thres) {
      err++;
      printf("(golden, hw)@(%d): (%f, %f)\n", i, (float)fc_cout_golden[i],
             (float)fc_cout[i]);
    }

  if (err) {
    printf("Test failed with %d errors!\n", err);
    return -1;
  } else {
    printf("Test passed!\n");
    return 0;
  }
}