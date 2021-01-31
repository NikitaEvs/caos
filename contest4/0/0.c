#include<stdio.h>
#include<stdlib.h>

extern void summ(int N, const int *A, const int *B, int *R);


int main() {
  int* a = (int*)malloc(5*sizeof(int));
  int* b = (int*)malloc(5*sizeof(int));
  int* R = (int*)malloc(5*sizeof(int));

  for (int i = 0; i < 5; ++i) {
    a[i] = i;
    b[i] = i;
    R[i] = 0;
  }

  summ(1, a, b, R);

  for (int i = 0; i < 5; ++i) {
    printf("%d ", R[i]);
  }

  return 0;
}
