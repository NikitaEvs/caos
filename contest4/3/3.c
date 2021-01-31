#include <stdio.h>
#include <stdlib.h>

extern float dot_product(int N, const float *A, const float *B);

int main() {
  int N = 500000;
  float* A = (float*)malloc(N*sizeof(float));
  float* B = (float*)malloc(N*sizeof(float));

  for (int i = 0; i < N; ++i) {
      A[i] = 1.0;
      B[i] = 1.0;
  }

  float res = dot_product(N, A, B);

  printf("%f", res); 

  return 0;
}
